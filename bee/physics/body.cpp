/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_PHYSICS_BODY
#define _BEE_PHYSICS_BODY 1

#include "body.hpp"

namespace bee {
	PhysicsBody::PhysicsBody(PhysicsWorld* new_world, Instance* new_inst, E_PHYS_SHAPE new_type, double new_mass, double x, double y, double z, double* p) {
		attached_world = new_world;
		attached_instance = new_inst;

		type = new_type;
		mass = new_mass;

		if (motion_state != nullptr) {
			delete motion_state;
			motion_state = nullptr;
		}
		if (body != nullptr) {
			delete body;
			body = nullptr;
		}

		set_shape(new_type, p);

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(btVector3(x, y, z)/scale);
		motion_state = new btDefaultMotionState(transform);

		btRigidBody::btRigidBodyConstructionInfo rb_info (btScalar(mass), motion_state, shape, get_inertia());
		rb_info.m_friction = friction;

		body = new btRigidBody(rb_info);

		body->setSleepingThresholds(body->getLinearSleepingThreshold()/scale, body->getAngularSleepingThreshold());
	}
	PhysicsBody::~PhysicsBody() {
		delete motion_state;

		remove();
		remove_constraints();
		delete body;

		if (shape_params != nullptr) {
			delete[] shape_params;
			shape_params = nullptr;
		}

		delete shape;
	}

	std::string PhysicsBody::serialize(bool should_pretty_print) const {
		std::vector<SIDP>* sp = new std::vector<SIDP>();
		for (size_t i=0; i<shape_param_amount; ++i) {
			(*sp).push_back(shape_params[i]);
		}

		std::vector<SIDP>* cons = new std::vector<SIDP>();
		for (auto& c : constraints) {
			size_t constraint_param_amount = attached_world->get_constraint_param_amount(std::get<0>(c));
			std::vector<SIDP>* con = new std::vector<SIDP>();

			for (size_t i=0; i<constraint_param_amount; ++i) {
				(*con).push_back(std::get<1>(c)[i]);
			}

			(*cons).push_back(SIDP().vector(con));
		}

		std::map<std::string,SIDP> data;
		data["type"] = static_cast<int>(type);
		data["mass"] = mass;
		data["scale"] = scale;
		data["friction"] = friction;
		data["shape_params"].vector(sp);

		data["attached_instance"] = attached_instance->id;
		data["position"].vector(new std::vector<SIDP>({get_position().x(), get_position().y(), get_position().z()}));
		data["rotation"].vector(new std::vector<SIDP>({get_rotation_x(), get_rotation_y(), get_rotation_z()}));

		data["gravity"].vector(new std::vector<SIDP>({body->getGravity().x(), body->getGravity().y(), body->getGravity().z()}));
		data["velocity"].vector(new std::vector<SIDP>({body->getLinearVelocity().x(), body->getLinearVelocity().y(), body->getLinearVelocity().z()}));
		data["velocity_ang"].vector(new std::vector<SIDP>({body->getAngularVelocity().x(), body->getAngularVelocity().y(), body->getAngularVelocity().z()}));

		data["collision_flags"] = body->getCollisionFlags();
		data["constraints"].vector(cons);

		return map_serialize(data, should_pretty_print);
	}
	std::string PhysicsBody::serialize() const {
		return serialize(false);
	}
	int PhysicsBody::deserialize(const std::string& data) {
		std::map<std::string,SIDP> m;
		map_deserialize(data, &m);

		type = static_cast<E_PHYS_SHAPE>(SIDP_i(m["type"]));
		mass = SIDP_d(m["mass"]);
		scale = SIDP_d(m["scale"]);
		friction = SIDP_d(m["friction"]);
		shape_param_amount = get_shape_param_amount(type);
		if ((type == E_PHYS_SHAPE::MULTISPHERE)||(type == E_PHYS_SHAPE::CONVEX_HULL)) {
			shape_param_amount = get_shape_param_amount(type, SIDP_cd(m["shape_params"], 0));
		}

		if (shape_params != nullptr) {
			delete[] shape_params;
			shape_params = nullptr;
		}
		if (shape_param_amount > 0) {
			shape_params = new double[shape_param_amount];
			for (size_t i=0; i<shape_param_amount; ++i) {
				shape_params[i] = SIDP_cd(m["shape_params"], i);
			}
		}

		attached_instance = get_current_room()->get_instances().at(SIDP_i(m["attached_instance"]));
		body->setCollisionFlags(SIDP_i(m["collision_flags"]));

		set_shape(type, shape_params);

		btVector3 position = btVector3(SIDP_cd(m["position"], 0), SIDP_cd(m["position"], 1), SIDP_cd(m["position"], 2));
		btVector3 rotation = btVector3(SIDP_cd(m["rotation"], 0), SIDP_cd(m["rotation"], 1), SIDP_cd(m["rotation"], 2));

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(position/scale);
		btQuaternion qt;
		qt.setEuler(rotation.y(), rotation.x(), rotation.z());
		transform.setRotation(qt);
		body->setCenterOfMassTransform(transform);

		btVector3 gravity = btVector3(SIDP_cd(m["gravity"], 0), SIDP_cd(m["gravity"], 1), SIDP_cd(m["gravity"], 2));
		body->setGravity(gravity);

		btVector3 velocity = btVector3(SIDP_cd(m["velocity"], 0), SIDP_cd(m["velocity"], 1), SIDP_cd(m["velocity"], 2));
		btVector3 velocity_ang = btVector3(SIDP_cd(m["velocity_ang"], 0), SIDP_cd(m["velocity_ang"], 1), SIDP_cd(m["velocity_ang"], 2));
		body->setLinearVelocity(velocity);
		body->setAngularVelocity(velocity_ang);

		return 0;
	}

	int PhysicsBody::attach(PhysicsWorld* new_world) {
		if (attached_instance != nullptr) {
			attached_instance->add_physbody();
		}
		/*if (attached_world != new_world)*/ {
			attached_world = new_world;

			if (constraints.size() > 0) {
				auto tmp_constraints = constraints;
				constraints.clear();
				for (auto& c : tmp_constraints) {
					attached_world->add_constraint(std::get<0>(c), this, std::get<1>(c));
				}
			}
		}

		return 0;
	}
	int PhysicsBody::remove() {
		if (attached_world != nullptr) {
			for (auto& c : constraints) {
				if (std::get<2>(c) != nullptr) {
					attached_world->remove_constraint(std::get<2>(c));

					delete std::get<2>(c);
					std::get<2>(c) = nullptr;
				}
			}

			attached_world->remove_body(this);
			attached_world = nullptr;
		}

		return 0;
	}

	double PhysicsBody::get_mass() const {
		return mass;
	}
	double PhysicsBody::get_scale() const {
		return scale;
	}
	btVector3 PhysicsBody::get_inertia() const {
		btVector3 local_intertia (0.0, 0.0, 0.0);
		if ((mass != 0.0)&&(shape != nullptr)) {
			shape->calculateLocalInertia(btScalar(mass), local_intertia);
		}
		return local_intertia*scale;
	}
	btRigidBody* PhysicsBody::get_body() const {
		return body;
	}
	PhysicsWorld* PhysicsBody::get_world() const {
		return attached_world;
	}
	const std::vector<std::tuple<E_PHYS_CONSTRAINT,double*,btTypedConstraint*>>& PhysicsBody::get_constraints() const {
		return constraints;
	}

	btDefaultMotionState* PhysicsBody::get_motion() const {
		return motion_state;
	}
	btVector3 PhysicsBody::get_position() const {
		return body->getCenterOfMassPosition()*scale;
	}
	btQuaternion PhysicsBody::get_rotation() const {
		return body->getCenterOfMassTransform().getRotation();
	}
	double PhysicsBody::get_rotation_x() const {
		return 2.0*asin(get_rotation().x());
	}
	double PhysicsBody::get_rotation_y() const {
		return 2.0*asin(get_rotation().y());
	}
	double PhysicsBody::get_rotation_z() const {
		return 2.0*asin(get_rotation().z());
	}

	int PhysicsBody::set_shape(E_PHYS_SHAPE new_type, double* p) {
		if (shape != nullptr) {
			delete shape;
			shape = nullptr;
		}
		if ((shape_params != nullptr)&&(shape_params != p)) {
			delete[] shape_params;
			shape_params = nullptr;
		}
		type = new_type;
		shape_params = p;
		shape_param_amount = get_shape_param_amount(type);

		if (attached_world != nullptr) {
			scale = attached_world->get_scale();
		}

		switch (type) {
			case E_PHYS_SHAPE::SPHERE: {
				/*
				* p[0]: the radius
				*/
				shape = new btSphereShape(btScalar(p[0]));
				break;
			}
			case E_PHYS_SHAPE::BOX: {
				/*
				* p[0], p[1], p[2]: the width, height, and depth
				*/
				shape = new btBoxShape(btVector3(btScalar(p[0]), btScalar(p[1]), btScalar(p[2])) / (2.0*scale));
				break;
			}
			case E_PHYS_SHAPE::CYLINDER: {
				/*
				* p[0], p[1]: the radius and height
				*/
				shape = new btCylinderShape(btVector3(btScalar(p[0]), btScalar(p[1]), btScalar(p[0])));
				break;
			}
			case E_PHYS_SHAPE::CAPSULE: {
				/*
				* p[0], p[1]: the radius and height
				*/
				shape = new btCapsuleShape(btScalar(p[0]), btScalar(p[1]));
				break;
			}
			case E_PHYS_SHAPE::CONE: {
				/*
				* p[0], p[1]: the radius and height
				*/
				shape = new btConeShape(btScalar(p[0]), btScalar(p[1]));
				break;
			}
			case E_PHYS_SHAPE::MULTISPHERE: {
				/*
				* p[0]: the amount of spheres
				* p[1], p[2], p[3], ..., p[p[0]]: the x-, y-, and z-coordinates of each sphere's center
				* p[p[0]+1], p[p[0]+2], p[p[0]+3], ..., p[2*p[0]]: the x-, y-, and z-scaling for each axis of the appropriate sphere
				*/
				size_t amount = p[0];

				btVector3* pos = new btVector3[amount];
				for (size_t i=0; i<amount; i+=3) {
					pos[i] = btVector3(p[i+2], p[i+3], p[i+4]);
				}
				btScalar* radii = new btScalar[amount];
				for (size_t i=0; i<amount; ++i) {
					radii[i] = btScalar(p[i+amount+1]);
				}

				shape_param_amount = get_shape_param_amount(type, p[0]);
				shape = new btMultiSphereShape(pos, radii, amount);

				delete[] pos;
				delete[] radii;

				break;
			}
			case E_PHYS_SHAPE::CONVEX_HULL: {
				/*
				* p[0]: the amount of points
				* p[1], p[2], p[3], ..., p[p[0]]: the x-, y-, and z-coordinates of each point
				*/
				size_t amount = p[0];

				btConvexHullShape* tmp_shape = new btConvexHullShape();

				for (size_t i=0; i<amount; i+=3) {
					tmp_shape->addPoint(btVector3(p[i+2], p[i+3], p[i+4]));
				}

				shape_param_amount = get_shape_param_amount(type, p[0]);
				shape = tmp_shape;

				break;
			}

			default:
				std::cerr << "PHYS ERR invalid shape type\n";
				/* FALL THROUGH */
			case E_PHYS_SHAPE::NONE:
				shape_param_amount = 0;
				shape = new btEmptyShape();
		}

		update_state();

		return 0;
	}
	int PhysicsBody::set_mass(double new_mass) {
		mass = new_mass;

		update_state();

		return 0;
	}
	int PhysicsBody::set_friction(double new_friction) {
		friction = new_friction;

		update_state();

		return 0;
	}

	int PhysicsBody::add_constraint(E_PHYS_CONSTRAINT type, double* p) {
		if (attached_world != nullptr) {
			attached_world->add_constraint(type, this, p);
		} else {
			constraints.emplace_back(type, p, nullptr);
		}

		return 0;
	}
	int PhysicsBody::add_constraint_external(E_PHYS_CONSTRAINT type, double* p, btTypedConstraint* constraint) {
		constraints.emplace_back(type, p, constraint);
		return 0;
	}
	int PhysicsBody::remove_constraints() {
		while (body->getNumConstraintRefs()) {
			btTypedConstraint* c = body->getConstraintRef(0);
			if (attached_world != nullptr) {
				attached_world->remove_constraint(c);
			}
			body->removeConstraintRef(c);
			delete c;
		}

		for (auto& c : constraints) {
			if (std::get<1>(c) != nullptr) {
				delete std::get<1>(c);
			}
		}
		constraints.clear();

		return 0;
	}

	int PhysicsBody::update_state() {
		if ((body == nullptr)||(motion_state == nullptr)) {
			return 1;
		}

		PhysicsWorld* tmp_world = attached_world;
		remove();

		int cflags = body->getCollisionFlags();

		delete body;
		body = nullptr;

		btRigidBody::btRigidBodyConstructionInfo rb_info (btScalar(mass), motion_state, shape, get_inertia());
		rb_info.m_friction = friction;

		body = new btRigidBody(rb_info);

		body->setSleepingThresholds(body->getLinearSleepingThreshold()/scale, body->getAngularSleepingThreshold());
		body->setCollisionFlags(body->getCollisionFlags() | (cflags & btCollisionObject::CF_NO_CONTACT_RESPONSE));

		attached_world = tmp_world;
		attached_world->add_body(this);

		return 0;
	}

	size_t PhysicsBody::get_shape_param_amount(E_PHYS_SHAPE s, int p0) const {
		switch (s) {
			case E_PHYS_SHAPE::SPHERE:      return 1;
			case E_PHYS_SHAPE::BOX:         return 3;

			case E_PHYS_SHAPE::CYLINDER:
			case E_PHYS_SHAPE::CAPSULE:
			case E_PHYS_SHAPE::CONE:        return 2;

			case E_PHYS_SHAPE::MULTISPHERE: return p0 * 4 + 1;
			case E_PHYS_SHAPE::CONVEX_HULL: return p0 * 3 + 1;

			default:
			case E_PHYS_SHAPE::NONE:        return 0;
		}
	}
	size_t PhysicsBody::get_shape_param_amount(E_PHYS_SHAPE s) const {
		return get_shape_param_amount(s, 0);
	}
}

#endif // _BEE_PHYSICS_BODY

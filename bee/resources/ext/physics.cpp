/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_PHYSICS
#define _BEE_PHYSICS 1

#include "physics.hpp"

BEE::PhysicsWorld::PhysicsWorld() {
	init(gravity, scale);
}
BEE::PhysicsWorld::PhysicsWorld(btVector3 new_gravity, double new_scale) {
	init(new_gravity, new_scale);
}
BEE::PhysicsWorld::~PhysicsWorld() {
	for (int i=world->getNumConstraints()-1; i>=0; --i) {
		btTypedConstraint* c = world->getConstraint(i);
		world->removeConstraint(c);
		delete c;
	}

	delete debug_draw;

	delete world;
	delete solver;
	delete broadphase;
	delete dispatcher;
	delete collision_configuration;
}

int BEE::PhysicsWorld::init(btVector3 new_gravity, double new_scale) {
	collision_configuration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collision_configuration);
	broadphase = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver;
	world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collision_configuration);

	debug_draw = new PhysicsDraw();
	debug_draw->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	world->setDebugDrawer(debug_draw);

	set_gravity(new_gravity);
	set_scale(scale);

	return 0;
}
int BEE::PhysicsWorld::attach(BEE* new_game) {
	debug_draw->attach(new_game, this);
	return 0;
}

btVector3 BEE::PhysicsWorld::get_gravity() const {
	return gravity;
}
double BEE::PhysicsWorld::get_scale() const {
	return scale;
}
btDispatcher* BEE::PhysicsWorld::get_dispatcher() const {
	return world->getDispatcher();
}

int BEE::PhysicsWorld::set_gravity(btVector3 new_gravity) {
	gravity = new_gravity;
	//world->setGravity(gravity);
	world->setGravity(10.0*gravity/scale);
	return 0;
}
int BEE::PhysicsWorld::set_scale(double new_scale) {
	if (new_scale != scale) {
		for (int i=world->getNumCollisionObjects()-1; i>=0; --i) {
			btCollisionObject* obj = world->getCollisionObjectArray()[i];
			btRigidBody* body = btRigidBody::upcast(obj);

			if ((body)&&(body->getMotionState())) {
				delete body->getMotionState();
			}

			world->removeCollisionObject(obj);
			delete obj;
		}
	}

	scale = new_scale;
	world->setGravity(10.0*gravity/scale);

	return 0;
}

int BEE::PhysicsWorld::add_body(PhysicsBody* new_body) {
	if (scale != new_body->get_scale()) {
		std::cerr << "PHYS WARN failed to add body to world: scale mismatch: world~" << scale << ", body~" << new_body->get_scale() << "\n";
		return 1;
	}

	world->addRigidBody(new_body->get_body());
	new_body->attach(this);

	return 0;
}
int BEE::PhysicsWorld::add_constraint(bee_phys_constraint_t type, PhysicsBody* body, double* p) {
	switch (type) {
		case BEE_PHYS_CONSTRAINT_POINT: {
			btPoint2PointConstraint* c = new btPoint2PointConstraint(*(body->get_body()), btVector3(p[0], p[1], p[2]));
			world->addConstraint(c);
			break;
		}
		case BEE_PHYS_CONSTRAINT_HINGE: {
			btHingeConstraint* c = new btHingeConstraint(*(body->get_body()), btVector3(p[0], p[1], p[2]), btVector3(p[3], p[4], p[5]));
			world->addConstraint(c);
			break;
		}
		case BEE_PHYS_CONSTRAINT_SLIDER: {
			btSliderConstraint* c = new btSliderConstraint(*(body->get_body()), btTransform::getIdentity(), true);
			c->setLowerLinLimit(p[0]);
			c->setUpperLinLimit(p[1]);
			c->setLowerAngLimit(p[2]);
			c->setUpperAngLimit(p[3]);
			world->addConstraint(c);
			break;
		}
		case BEE_PHYS_CONSTRAINT_CONE: {
			btConeTwistConstraint* c = new btConeTwistConstraint(*(body->get_body()), btTransform::getIdentity());
			c->setLimit(btScalar(p[0]), btScalar(p[1]), btScalar(p[2]), btScalar(p[3]));
			world->addConstraint(c);
			break;
		}
		case BEE_PHYS_CONSTRAINT_6DOF: {
			btGeneric6DofConstraint* c = new btGeneric6DofConstraint(*(body->get_body()), btTransform::getIdentity(), true);
			c->setLinearLowerLimit(btVector3(p[0], p[1], p[2]));
			c->setLinearUpperLimit(btVector3(p[3], p[4], p[5]));
			c->setAngularLowerLimit(btVector3(p[6], p[7], p[8]));
			c->setAngularUpperLimit(btVector3(p[9], p[10], p[11]));
			world->addConstraint(c);
			break;
		}
		case BEE_PHYS_CONSTRAINT_FIXED: {
			btGeneric6DofConstraint* c = new btGeneric6DofConstraint(*(body->get_body()), btTransform::getIdentity(), true);
			world->addConstraint(c);
			break;
		}
		case BEE_PHYS_CONSTRAINT_2D: {
			btGeneric6DofConstraint* c = new btGeneric6DofConstraint(*(body->get_body()), btTransform::getIdentity(), true);
			c->setLinearLowerLimit(btVector3(1, 1, 0));
			c->setLinearUpperLimit(btVector3(0, 0, 0));
			c->setAngularLowerLimit(btVector3(0, 0, 1));
			c->setAngularUpperLimit(btVector3(0, 0, 0));
			world->addConstraint(c);
			break;
		}
		default:
			std::cerr << "PHYS ERR invalid constraint type\n";
		case BEE_PHYS_CONSTRAINT_NONE:
			break;
	}
	return 0;
}
int BEE::PhysicsWorld::add_constraint(bee_phys_constraint_t type, PhysicsBody* body1, PhysicsBody* body2, double* p) {
	switch (type) {
		case BEE_PHYS_CONSTRAINT_POINT: {
			/*
			* p[0], p[1], p[2]:
			*/
			btPoint2PointConstraint* c = new btPoint2PointConstraint(*(body1->get_body()), *(body2->get_body()), btVector3(p[0], p[1], p[2]), btVector3(p[3], p[4], p[5]));
			world->addConstraint(c);
			break;
		}
		case BEE_PHYS_CONSTRAINT_HINGE: {
			btHingeConstraint* c = new btHingeConstraint(*(body1->get_body()), *(body2->get_body()), btVector3(p[0], p[1], p[2]), btVector3(p[3], p[4], p[5]), btVector3(p[6], p[7], p[8]), btVector3(p[9], p[10], p[11]));
			world->addConstraint(c);
			break;
		}
		case BEE_PHYS_CONSTRAINT_SLIDER: {
			btSliderConstraint* c = new btSliderConstraint(*(body1->get_body()), *(body2->get_body()), btTransform::getIdentity(), btTransform::getIdentity(), true);
			c->setLowerLinLimit(p[0]);
			c->setUpperLinLimit(p[1]);
			c->setLowerAngLimit(p[2]);
			c->setUpperAngLimit(p[3]);
			world->addConstraint(c, true);
			break;
		}
		case BEE_PHYS_CONSTRAINT_CONE: {
			btConeTwistConstraint* c = new btConeTwistConstraint(*(body1->get_body()), *(body2->get_body()), btTransform::getIdentity(), btTransform::getIdentity());
			c->setLimit(btScalar(p[0]), btScalar(p[1]), btScalar(p[2]), btScalar(p[3]));
			world->addConstraint(c, true);
			break;
		}
		case BEE_PHYS_CONSTRAINT_6DOF: {
			btGeneric6DofConstraint* c = new btGeneric6DofConstraint(*(body1->get_body()), *(body2->get_body()), btTransform::getIdentity(), btTransform::getIdentity(), true);
			c->setLinearLowerLimit(btVector3(p[0], p[1], p[2]));
			c->setLinearUpperLimit(btVector3(p[3], p[4], p[5]));
			c->setAngularLowerLimit(btVector3(p[6], p[7], p[8]));
			c->setAngularUpperLimit(btVector3(p[9], p[10], p[11]));
			world->addConstraint(c, true);
			break;
		}
		case BEE_PHYS_CONSTRAINT_FIXED: {
			//btFixedConstraint* c = new btFixedConstraint(*(body1->get_body()), *(body2->get_body()), btTransform::getIdentity(), btTransform::getIdentity());
			btGeneric6DofConstraint* c = new btGeneric6DofConstraint(*(body1->get_body()), *(body2->get_body()), btTransform::getIdentity(), btTransform::getIdentity(), true);
			world->addConstraint(c);
			break;
		}
		case BEE_PHYS_CONSTRAINT_2D: {
			std::cerr << "PHYS ERR CONSTRAINT_2D can only be used on a single rigid bodies\n";
		}
		default:
			std::cerr << "PHYS ERR invalid constraint type\n";
		case BEE_PHYS_CONSTRAINT_NONE:
			break;
	}
	return 0;
}

int BEE::PhysicsWorld::remove_body(PhysicsBody* body) {
	world->removeRigidBody(body->get_body());
	return 0;
}

int BEE::PhysicsWorld::step(double step_size) {
	world->stepSimulation(step_size, 10);
	return 0;
}

int BEE::PhysicsWorld::draw_debug() {
	world->debugDrawWorld();
	return 0;
}

BEE::PhysicsDraw::PhysicsDraw() {
	debug_mode = 0;
}
BEE::PhysicsDraw::~PhysicsDraw() {}
int BEE::PhysicsDraw::attach(BEE* new_game, PhysicsWorld* new_world) {
	attached_game = new_game;
	attached_world = new_world;
	return 0;
}

void BEE::PhysicsDraw::drawLine(const btVector3& v1, const btVector3& v2, const btVector3& color1, const btVector3& color2) {
	if (attached_game != nullptr) {
		double s = attached_world->get_scale();
		RGBA c (color1.x(), color1.y(), color1.z(), 255);
		attached_game->draw_line(bt_to_glm3(v1*s), bt_to_glm3(v2*s), c);
	}
}
void BEE::PhysicsDraw::drawLine(const btVector3& v1, const btVector3& v2, const btVector3& color) {
	if (attached_game != nullptr) {
		double s = attached_world->get_scale();
		RGBA c (color.x(), color.y(), color.z(), 255);
		attached_game->draw_line(bt_to_glm3(v1*s), bt_to_glm3(v2*s), c);
	}
}
void BEE::PhysicsDraw::drawSphere(const btVector3& center, const btVector3& radius, const btVector3& color) {
	if (attached_game != nullptr) {
		/*
		double s = attached_world->get_scale();
		RGBA c (color.x, color.y, color.z, 255)
		attached_game->draw_line(bt_to_glm3(v1*s), bt_to_glm3(v2*s), c);
		*/
	}
}
void BEE::PhysicsDraw::drawTriangle(const btVector3& v1, const btVector3& v2, const btVector3& v3, const btVector3& color, btScalar alpha) {
	if (attached_game != nullptr) {
		double s = attached_world->get_scale();
		RGBA c (color.x(), color.y(), color.z(), alpha);
		attached_game->draw_triangle(bt_to_glm3(v1*s), bt_to_glm3(v2*s), bt_to_glm3(v3*s), c, false);
	}
}
void BEE::PhysicsDraw::drawContactPoint(const btVector3& v1, const btVector3& normal, btScalar distance, int lifetime, const btVector3& color) {
	if (attached_game != nullptr) {
		double s = attached_world->get_scale();
		RGBA c (color.x(), color.y(), color.z(), 255);
		attached_game->draw_line(bt_to_glm3(v1*s), bt_to_glm3(distance*normal*s), c);
	}
}

void BEE::PhysicsDraw::reportErrorWarning(const char* str) {
	if (attached_game != nullptr) {
		attached_game->messenger_send({"engine", "physics", "draw"}, BEE_MESSAGE_WARNING, std::string(str));
	} else {
		std::cerr << "PHYS WARN: " << str << "\n";
	}
}

void BEE::PhysicsDraw::draw3dText(const btVector3&, const char*) {

}

void BEE::PhysicsDraw::setDebugMode(int new_debug_mode) {
	debug_mode = new_debug_mode;
}
int BEE::PhysicsDraw::getDebugMode() const {
	return debug_mode;
}

BEE::PhysicsBody::PhysicsBody(PhysicsWorld* new_world, bee_phys_shape_t new_type, double new_mass, double x, double y, double z, double* p) {
	attach(new_world);

	type = new_type;
	mass = new_mass;

	set_shape(new_type, p);

	if (motion_state != nullptr) {
		delete motion_state;
		motion_state = nullptr;
	}
	if (body != nullptr) {
		delete body;
		body = nullptr;
	}

	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(x, y, z)/scale);
	motion_state = new btDefaultMotionState(transform);

	btRigidBody::btRigidBodyConstructionInfo rb_info (btScalar(mass), motion_state, shape, get_inertia());
	body = new btRigidBody(rb_info);

	body->setSleepingThresholds(body->getLinearSleepingThreshold()/scale, body->getAngularSleepingThreshold());
}
BEE::PhysicsBody::~PhysicsBody() {
	delete shape;
	delete motion_state;
	delete body;
}

int BEE::PhysicsBody::attach(PhysicsWorld* new_world) {
	attached_world = new_world;
	return 0;
}

double BEE::PhysicsBody::get_mass() const {
	return mass;
}
double BEE::PhysicsBody::get_scale() const {
	return scale;
}
btVector3 BEE::PhysicsBody::get_inertia() const {
	btVector3 local_intertia (0.0, 0.0, 0.0);
	if (mass != 0.0) {
		shape->calculateLocalInertia(btScalar(mass), local_intertia);
	}
	return local_intertia*scale;
}
btRigidBody* BEE::PhysicsBody::get_body() const {
	return body;
}
BEE::PhysicsWorld* BEE::PhysicsBody::get_world() const {
	return attached_world;
}

btDefaultMotionState* BEE::PhysicsBody::get_motion() const {
	return motion_state;
}
btVector3 BEE::PhysicsBody::get_position() const {
	return body->getCenterOfMassPosition()*scale;
}
btQuaternion BEE::PhysicsBody::get_rotation() const {
	return body->getCenterOfMassTransform().getRotation();
}
double BEE::PhysicsBody::get_rotation_x() const {
	return 2.0*asin(get_rotation().x());
}
double BEE::PhysicsBody::get_rotation_y() const {
	return 2.0*asin(get_rotation().y());
}
double BEE::PhysicsBody::get_rotation_z() const {
	return 2.0*asin(get_rotation().z());
}

int BEE::PhysicsBody::set_shape(bee_phys_shape_t new_type, double* p) {
	if (shape != nullptr) {
		delete shape;
		shape = nullptr;
	}
	type = new_type;

	if (attached_world != nullptr) {
		scale = attached_world->get_scale();
	}

	switch (type) {
		case BEE_PHYS_SHAPE_SPHERE: {
			/*
			* p[0]: the radius
			*/
			shape = new btSphereShape(btScalar(p[0]));
			break;
		}
		case BEE_PHYS_SHAPE_BOX: {
			/*
			* p[0], p[1], p[2]: the width, height, and depth
			*/
			//shape = new btBoxShape(btVector3(btScalar(p[0]/2.0), btScalar(p[1]/2.0), btScalar(p[2]/2.0)));
			shape = new btBoxShape(btVector3(btScalar(p[0]), btScalar(p[1]), btScalar(p[2])) / (2.0*scale));
			break;
		}
		case BEE_PHYS_SHAPE_CYLINDER: {
			/*
			* p[0], p[1]: the radius and height
			*/
			shape = new btCylinderShape(btVector3(btScalar(p[0]), btScalar(p[1]), btScalar(p[0])));
			break;
		}
		case BEE_PHYS_SHAPE_CAPSULE: {
			/*
			* p[0], p[1]: the radius and height
			*/
			shape = new btCapsuleShape(btScalar(p[0]), btScalar(p[1]));
			break;
		}
		case BEE_PHYS_SHAPE_CONE: {
			/*
			* p[0], p[1]: the radius and height
			*/
			shape = new btConeShape(btScalar(p[0]), btScalar(p[1]));
			break;
		}
		case BEE_PHYS_SHAPE_MULTISPHERE: {
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

			shape = new btMultiSphereShape(pos, radii, amount);

			delete pos;
			delete radii;

			break;
		}
		case BEE_PHYS_SHAPE_CONVEX_HULL: {
			/*
			* p[0]: the amount of points
			* p[1], p[2], p[3], ..., p[p[0]]: the x-, y-, and z-coordinates of each point
			*/
			size_t amount = p[0];

			btConvexHullShape* tmp_shape = new btConvexHullShape();

			for (size_t i=0; i<amount; i+=3) {
				tmp_shape->addPoint(btVector3(p[i+2], p[i+3], p[i+4]));
			}

			shape = tmp_shape;

			break;
		}

		default:
			std::cerr << "PHYS ERR invalid shape type\n";
		case BEE_PHYS_SHAPE_NONE:
			shape = new btEmptyShape();
	}

	if ((motion_state != nullptr)&&(body != nullptr)) {
		if (attached_world != nullptr) {
			attached_world->remove_body(this);
		}

		delete body;
		body = nullptr;

		btRigidBody::btRigidBodyConstructionInfo rb_info (btScalar(mass), motion_state, shape, get_inertia());
		body = new btRigidBody(rb_info);

		body->setSleepingThresholds(body->getLinearSleepingThreshold()/scale, body->getAngularSleepingThreshold());

		if (attached_world != nullptr) {
			attached_world->add_body(this);
		}
	}

	return 0;
}
int BEE::PhysicsBody::set_mass(double new_mass) {
	mass = new_mass;

	if ((body != nullptr)&&(motion_state != nullptr)) {
		if (attached_world != nullptr) {
			attached_world->remove_body(this);
		}

		delete body;
		body = nullptr;

		btRigidBody::btRigidBodyConstructionInfo rb_info (btScalar(mass), motion_state, shape, get_inertia());
		body = new btRigidBody(rb_info);

		body->setSleepingThresholds(body->getLinearSleepingThreshold()/scale, body->getAngularSleepingThreshold());

		if (attached_world != nullptr) {
			attached_world->add_body(this);
		}
	}

	return 0;
}

#endif // _BEE_PHYSICS
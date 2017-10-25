//
//  ofxIKArm.cpp
//  emptyExample
//
//  Created by Nick Hardeman on 8/30/15.
//

#include "ofxIKArm.h"

//--------------------------------------------------------------
ofxIKArm::ofxIKArm() {
    shoulder    = shared_ptr< ofxJoint >( new ofxJoint() );
    elbow       = shared_ptr< ofxJoint >( new ofxJoint() );
    hand        = shared_ptr< ofxJoint >( new ofxJoint() );
    
    shoulder->localTransform.makeScaleMatrix( 1., 1., 1. );
    elbow->localTransform.makeScaleMatrix( 1., 1., 1. );
    hand->localTransform.makeScaleMatrix( 1., 1., 1.);
    
    elbow->parent   = shoulder;
    hand->parent    = elbow;
    
    shoulder->color.set( 20, 10, 220 );
    elbow->color.set( 60, 230, 0 );
    hand->color.set( 200, 200, 0 );
    
    hand->bIsEnd        = true;
    
    _saveFilePath       = "";
    armLength           = 0;
    bUpVectorEnabled    = false;
    mBWasSuccessful     = true;
}

//--------------------------------------------------------------
bool ofxIKArm::load( string aPath ) {
    _saveFilePath = aPath;
    if( _saveFilePath == "" ) {
        ofLogError("path empty please pass the load location as an argument to the save function." );
        return false;
    }
    if( !ofFile::doesFileExist(_saveFilePath) ) {
        return false;
    }
    ofBuffer tbuff = ofBufferFromFile( _saveFilePath );
    int lcount = 0;
    for( auto line : tbuff.getLines() ) {
//        cout << line << endl;
        if( lcount == 0 ) {
            shoulder->length = ofToFloat( line );
        } else if( lcount == 1 ) {
            elbow->length = ofToFloat( line );
        } else if( lcount == 2 ) {
            vector< string > tstrings = ofSplitString( line, "," );
            if( tstrings.size() == 3 ) {
                shoulder->localTransform.setTranslation(ofToFloat(tstrings[0]),
                                                        ofToFloat(tstrings[1]),
                                                        ofToFloat(tstrings[2]) );
            }
        }
        lcount++;
    }
    setup( shoulder->localTransform.getTranslation(), shoulder->length, elbow->length );
}

//--------------------------------------------------------------
bool ofxIKArm::save( string aPath ) {
    
    if( aPath != "" ) {
        _saveFilePath = aPath;
    }
    if( _saveFilePath == "" ) {
        ofLogError("path empty please pass the save location as an argument to the save function." );
        return false;
    }
    
    stringstream outStr;
    outStr << shoulder->length << "\n";
    outStr << elbow->length << "\n";
    outStr << shoulder->getGlobalPosition().x << ","<< shoulder->getGlobalPosition().y << "," << shoulder->getGlobalPosition().z;
    ofBuffer tbuff;
    tbuff.append( outStr.str() );
    return ofBufferToFile( _saveFilePath, tbuff );
}

//--------------------------------------------------------------
void ofxIKArm::setup( ofVec3f aGlobalShoulderPos, float aUpperArmLength, float aLowerArmLength ) {
    shoulder->localTransform.setTranslation( aGlobalShoulderPos );
    elbow->localTransform.setTranslation( 0, 0, aUpperArmLength );
    hand->localTransform.setTranslation( 0, 0, aLowerArmLength );
    
    shoulder->length    = aUpperArmLength;
	elbow->length       = aLowerArmLength;
	armLength           = shoulder->length + elbow->length;
    
    setDrawSize( 1 );
}

//--------------------------------------------------------------
void ofxIKArm::setup( ofVec3f aGlobalShoulderPos, ofVec3f aGlobalElbowPos, ofVec3f aGlobalHandPos ) {
    float tupperLen = (aGlobalElbowPos - aGlobalShoulderPos).length();
    float tlowerLen = (aGlobalHandPos - aGlobalElbowPos).length();
    setup( aGlobalShoulderPos, tupperLen, tlowerLen );
}

//--------------------------------------------------------------
void ofxIKArm::update() {
    
//    bool bInvert = ofGetKeyPressed('i');
    
    mBWasSuccessful = true;
    
    ofQuaternion prevShouldQ = shoulder->getGlobalTransform().getRotate();
    ofQuaternion prevElbowQ = elbow->localTransform.getRotate();
    
    ofVec3f shoulderPos     = shoulder->getGlobalPosition();
    ofVec3f shoulderDiff    = shoulderPos - target;
    float distToShoulder    = shoulderDiff.length();
    
    ofVec3f selbowTarDiff   = elbowtarget - shoulder->getGlobalPosition();
    
    float s1 = distToShoulder;
    float s2 = elbow->length;
    float s3 = shoulder->length;
    float fDegrees = (float) acos( (s2 * s2 + s3 * s3 - s1 * s1) / (2 * s2 * s3) ) * ( 180.0f / 3.1415f );
    fDegrees += 180.;
    
//        cout << "fDegrees: " << fDegrees << " nan: " << isnan(fDegrees) << endl;
    if( fDegrees < 180 ) {
        fDegrees = 180;
    }
    
    
    if( isnan(fDegrees) ) {
        fDegrees = 182;
        mBWasSuccessful = false;
    }
    
    if( distToShoulder >= armLength ) {
        fDegrees = 4;
    }
    
    ofQuaternion equat;
//    equat.makeRotate( fDegrees, ofVec3f(1,0,0) );
    if( isInverted() ) fDegrees *= -1.f;
    equat.makeRotate( fDegrees, ofVec3f(1,0,0) );
    
    elbow->localTransform.setRotate( equat );
    
    
    
    s2 = s1;
    s1 = elbow->length;
    s3 = shoulder->length;
    float sDegrees = (float) acos ( (s2 * s2 + s3 * s3 - s1 * s1) / (2 * s2 * s3) ) * ( 180.0f / 3.1415f );
    
    //    cout << "sDegrees: " << sDegrees << " nan: " << isnan(sDegrees) << endl;
    
    if( isnan(sDegrees) ) {
        sDegrees = 0.;
        mBWasSuccessful = false;
    }
    
    if( distToShoulder >= armLength ) {
        sDegrees = 0.;
    }
    if( isInverted() ) sDegrees = -sDegrees;
    
    ofQuaternion shoulderQuat;
    ofVec3f shoulderToTarget = target - shoulderPos;
    shoulderToTarget.normalize();
    
    if( isShoulderUpVectorEnabled() ) {
        ofNode tnode;
        ofVec3f telbowDiff = shoulderToTarget - elbowtarget;
        telbowDiff = shoulderPos - elbowtarget;
//        telbowDiff.y = 0;
        
        tnode.lookAt( -ofVec3f(shoulderToTarget.x, shoulderToTarget.y, shoulderToTarget.z), telbowDiff.getNormalized() );
//        tnode.lookAt( -ofVec3f(shoulderToTarget.x, shoulderToTarget.y, shoulderToTarget.z), ofVec3f(0,1,0) );
//        tnode.roll(90);
        shoulderQuat = tnode.getOrientationQuat();
//        ofVec3f teuler = shoulderQuat.getEuler();
//        shoulderQuat = getQuatFromEuler( ofDegToRad( teuler.y ), ofDegToRad( teuler.x ), ofDegToRad( 0 ) );
        
//        float yRotAtan = atan2(shoulderDiff.x, shoulderDiff.z);
//        shoulderQuat.makeRotate( ofRadToDeg(yRotAtan), 0, 1, 0 );
        
//        ofQuaternion xrot;
//        xrot.makeRotate( tnode.getPitch(), -1., 0., 0 );
//        shoulderQuat = xrot * shoulderQuat;
        
//        float xRotATan = atan2(shoulderDiff.y, shoulderDiff.z );
//        float eXRotATan = atan2(selbowTarDiff.getNormalized().y, selbowTarDiff.getNormalized().z );
//        xrot.makeRotate( ofRadToDeg((xRotATan)), 1., 0, 0 );
        
        ofVec2f yzShoulderDiff( shoulderDiff.y, shoulderDiff.z );
        float trotX = atan2( shoulderDiff.y, shoulderDiff.z );
        
        if( isnan( trotX ) ) {
            trotX = 0.;
        }
        
        ofQuaternion trotQX;
        trotQX.makeRotate( ofRadToDeg(PI-trotX), 1, 0, 0 );
//        trotQX.makeRotate( -ofRadToDeg(trotX)+180, 1, 0, 0 );
        
        shoulderQuat = trotQX;
        
        // *****
//        ofNode tnode;
//        tnode.lookAt( -shoulderToTarget, mShoulderUpVec );
//        shoulderQuat = tnode.getOrientationQuat();
        // *****
        
        
//        shoulderQuat = xrot * shoulderQuat;
        
        
//        tnode.lookAt(shoulderPos - elbowtarget, ofVec3f(0,1,0) );
        // now figure out the amount of rotation around the x axis //
//        ofQuaternion zrot, xrot;
//        xrot.makeRotate( tnode.getPitch(), 1., 0.0, 0.0 );
//        zrot.makeRotate( tnode.getRoll(), 0, 0, -1 );
//        shoulderQuat = zrot * shoulderQuat;
        
        
    } else {
        shoulderQuat.makeRotate( ofVec3f(0,0,1), shoulderToTarget );
    }
    
    ofQuaternion squat;
    squat.makeRotate( sDegrees, ofVec3f(1,0,0) );
    
//    cout << "trying to rotate by the sDegrees " << sDegrees << " | " << ofGetFrameNum() << endl;

//    shoulder->localTransform.setRotate( shoulderQuat );
    shoulder->localTransform.setRotate( squat * shoulderQuat );
    
//    shoulder->localTransform.setRotate( squat );
    
    
    if( !isShoulderUpVectorEnabled() ) {
        ofVec3f shandDiff = shoulder->getGlobalPosition() - hand->getGlobalPosition();
        shandDiff.normalize();
        
        ofVec3f lzaxis = shoulder->localTransform.getRotate() * ofVec3f(1,0,0);
        ofVec3f tcross = shandDiff.getCrossed( selbowTarDiff );
        ofQuaternion ytrot;
        
        float tcdot = lzaxis.dot( selbowTarDiff );
        //    cout << "tcdot: " << tcdot << endl;
        
        float seangle = tcross.angle( lzaxis )-180;
        ytrot.makeRotate( seangle * ofSign( tcdot ), shandDiff );
    
        shoulder->localTransform.setRotate( shoulder->localTransform.getRotate() * ytrot );
    
        ofQuaternion desiredRot = shoulder->localTransform.getRotate() * ytrot;
        ofQuaternion newRot;
        ofVec3f teuler = desiredRot.getEuler();
//        teuler = getYawPitchRoll( desiredRot );
//        teuler *= RAD_TO_DEG;
//        newRot.makeRotate(teuler.x, ofVec3f(1,0,0), teuler.y, ofVec3f(0,1,0), teuler.z, ofVec3f(0,0,1) );
//        getQuatFromEuler(double pitch, double roll, double yaw) {
        newRot = getQuatFromEuler( ofDegToRad( teuler.x ), ofDegToRad(teuler.y), ofDegToRad(teuler.z) );
        newRot = getQuatFromEuler( ofDegToRad( teuler.z ), ofDegToRad(teuler.y), ofDegToRad(teuler.y) );
        newRot = getQuatFromEuler( ofDegToRad( teuler.y ), ofDegToRad(teuler.x), ofDegToRad(teuler.z) );
        newRot = getQuatFromEuler( ofDegToRad( teuler.y ), ofDegToRad(teuler.x), ofDegToRad(0) );
        
        ofQuaternion xrot, yrot, zrot;
        xrot.makeRotate( teuler.x, 1.f, 0.0, 0.0 );
        yrot.makeRotate( teuler.y, 0.0, 1.0, 0.0 );
        zrot.makeRotate( teuler.z, 0.0, 0.0, 1.0 );
        
//        shoulder->localTransform.setRotate( newRot );
    
        // new approach, don't set z at all //
//        ofQuaternion tyLookAt = getLookAt( ofVec3f( shoulderDiff.x, 0.0, shoulderDiff.z ), ofVec3f(0,1,0) );
    }
    
    if( !mBWasSuccessful ) {
//        shoulder->localTransform.setRotate( prevShouldQ );
//        elbow->localTransform.setRotate( prevElbowQ );
    }
}

//--------------------------------------------------------------
void ofxIKArm::draw() {
    
    ofSetColor( 255, 0, 0 );
    ofDrawSphere( target, shoulder->getDrawSize()*1.3 );
    ofDrawLine( target, shoulder->getGlobalPosition() );
    
    ofSetColor( 120 );
    ofDrawLine( shoulder->getGlobalPosition(), hand->getGlobalPosition() );
    
//    if( !isShoulderUpVectorEnabled() ) {
        ofSetColor( 255, 220, 0 );
        ofDrawLine( elbowtarget, elbow->getGlobalPosition() );
        ofSetColor( 200, 30, 200 );
        ofDrawSphere( elbowtarget, elbow->getDrawSize()*0.3 );
//    }
    
    shoulder->draw();
    elbow->draw();
    hand->draw();
}

//--------------------------------------------------------------
void ofxIKArm::setTarget( ofVec3f aTarget ) {
    target.set( aTarget );
}

//--------------------------------------------------------------
void ofxIKArm::setTarget( float ax, float ay, float az ) {
    target.set( ax, ay, az );
}

//--------------------------------------------------------------
void ofxIKArm::setElbowTarget( ofVec3f aTarget ) {
    elbowtarget.set( aTarget );
}

//--------------------------------------------------------------
void ofxIKArm::setElbowTarget( float ax, float ay, float az ) {
    elbowtarget.set( ax, ay, az );
}

//--------------------------------------------------------------
void ofxIKArm::setShoulderPos( ofVec3f aPos ) {
    shoulderPos = aPos;
    shoulder->localTransform.setTranslation( aPos );
}

//--------------------------------------------------------------
void ofxIKArm::setUpperArmLength( float aLen ) {
    setArmLengths( aLen, elbow->length );
}

//--------------------------------------------------------------
void ofxIKArm::setLowerArmLength( float aLen ) {
	setArmLengths( shoulder->length, aLen );
}

//--------------------------------------------------------------
void ofxIKArm::setArmLengths( float aUpperLen, float aLowerLen ) {
    shoulder->length    = aUpperLen;
    elbow->length       = aLowerLen;
    armLength           = shoulder->length + elbow->length;
    elbow->localTransform.setTranslation( shoulder->length, 0, 0 );
    hand->localTransform.setTranslation( elbow->length, 0, 0 );
    shoulder->setDrawSize( shoulder->getDrawSize(), true );
    elbow->setDrawSize( elbow->getDrawSize(), true );
}

//--------------------------------------------------------------
void ofxIKArm::setDrawSize( float aSize ) {
    shoulder->setDrawSize(aSize);
    elbow->setDrawSize( aSize );
    hand->setDrawSize( aSize );
}

//--------------------------------------------------------------
float ofxIKArm::getArmLength() {
    return armLength;
}

//--------------------------------------------------------------
float ofxIKArm::getUpperArmLength() {
    return shoulder->length;
}

//--------------------------------------------------------------
float ofxIKArm::getLowerArmLength() {
    return elbow->length;
}

//--------------------------------------------------------------
shared_ptr< ofxJoint > ofxIKArm::getShoulderJoint() {
    return shoulder;
}

//--------------------------------------------------------------
shared_ptr< ofxJoint > ofxIKArm::getElbowJoint() {
    return elbow;
}

//--------------------------------------------------------------
shared_ptr< ofxJoint > ofxIKArm::getHandJoint() {
    return hand;
}

//--------------------------------------------------------------
ofVec3f ofxIKArm::getTarget() {
    return target;
}

//--------------------------------------------------------------
ofVec3f ofxIKArm::getElbowTarget() {
    return elbowtarget;
}

//--------------------------------------------------------------
ofVec3f ofxIKArm::getShoulderPos() {
    return shoulderPos;
}

//--------------------------------------------------------------
ofVec3f ofxIKArm::getShoulderGlobalPos() {
    return shoulder->getGlobalPosition();
}

//--------------------------------------------------------------
ofVec3f ofxIKArm::getElbowGlobalPos() {
    return elbow->getGlobalPosition();
}

//--------------------------------------------------------------
ofVec3f ofxIKArm::getHandGlobalPos() {
    return hand->getGlobalPosition();
}

// if this is set, then the elbow target will not work
//--------------------------------------------------------------
void ofxIKArm::setShoulderUpVector( ofVec3f aShoulderUpVec ) {
    mShoulderUpVec = aShoulderUpVec;
}

//--------------------------------------------------------------
void ofxIKArm::setShoulderUpVectorEnabled( bool ab ) {
    bUpVectorEnabled = ab;
}

//--------------------------------------------------------------
bool ofxIKArm::isShoulderUpVectorEnabled() {
    return bUpVectorEnabled;
}

//--------------------------------------------------------------
ofVec3f ofxIKArm::getShoulderUpVector() {
    return mShoulderUpVec;
}

//--------------------------------------------------------------
bool ofxIKArm::isInverted() {
    return mInverted;
}

//--------------------------------------------------------------
void ofxIKArm::setInverted( bool ab ) {
    mInverted = ab;
}

//------------------------------------------------------------------
ofVec3f ofxIKArm::getYawPitchRoll( ofQuaternion aquat ) {
    float qx = aquat.x();
    float qy = aquat.y();
    float qz = aquat.z();
    float qw = aquat.w();
    
    float yaw   =  atan2(2*qx*qy + 2*qw*qz, qw*qw + qx*qx - qy*qy - qz*qz);
    float pitch = -asin(2*qw*qy - 2*qx*qz);
    float roll  = -atan2(2*qy*qz + 2*qw*qx, -qw*qw + qx*qx + qy*qy - qz*qz);
    
    return ofVec3f( yaw, pitch, roll );
}

// https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
//------------------------------------------------------------------
ofQuaternion ofxIKArm::getQuatFromEuler(double pitch, double roll, double yaw) {
    double t0 = std::cos(yaw * 0.5f);
    double t1 = std::sin(yaw * 0.5f);
    double t2 = std::cos(roll * 0.5f);
    double t3 = std::sin(roll * 0.5f);
    double t4 = std::cos(pitch * 0.5f);
    double t5 = std::sin(pitch * 0.5f);
    
    double qw = t0 * t2 * t4 + t1 * t3 * t5;
    double qx = t0 * t3 * t4 - t1 * t2 * t5;
    double qy = t0 * t2 * t5 + t1 * t3 * t4;
    double qz = t1 * t2 * t4 - t0 * t3 * t5;
    return ofQuaternion( qx, qy, qz, qw );
}

// from ofNode //
ofQuaternion ofxIKArm::getLookAt( ofVec3f aLookDir, ofVec3f aUpVec ) {
    ofVec3f zaxis = aLookDir.getNormalized();// (getGlobalPosition() - lookAtPosition).getNormalized();
    if (zaxis.length() > 0) {
        ofVec3f xaxis = aUpVec.getCrossed(zaxis).getNormalized();
        ofVec3f yaxis = zaxis.getCrossed(xaxis);
        
        ofMatrix4x4 m;
        m._mat[0].set(xaxis.x, xaxis.y, xaxis.z, 0);
        m._mat[1].set(yaxis.x, yaxis.y, yaxis.z, 0);
        m._mat[2].set(zaxis.x, zaxis.y, zaxis.z, 0);
        
//        setGlobalOrientation(m.getRotate());
        return m.getRotate();
    }
    return ofQuaternion();
}













#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    cam.disableMouseInput();
    cam.setDistance( 20 );
    cam.setPosition( 0, 0, 20 );
    cam.lookAt( ofVec3f(0,0,0));
    ofSetFrameRate( 60 );
    
//    arm.setup( ofVec3f(3,0,0), ofVec3f(4,-5,0), ofVec3f(5, -10, 0) );
    arm.setup( ofVec3f(0,0,0), 7, 9 );
    arm.setDrawSize( 1.2 );
    
    arm.setShoulderUpVectorEnabled( true );
    arm.setShoulderUpVector( ofVec3f(0,0,-1) );
}

//--------------------------------------------------------------
void ofApp::update() {
    
//    arm.setTarget(ofMap( ofGetMouseX(), 0, ofGetWidth(), -16, 16, true ),
//                  ofMap( ofGetMouseY(), 0, ofGetHeight(), 10, -10, true ),
//                  arm.getTarget().z );
    
    ofVec3f mousePos( ofGetMouseX(), ofGetMouseY() );
    mousePos = cam.screenToWorld( mousePos );
//    arm.setTarget( mousePos );
//    arm.setTarget( ofMap( ofGetMouseX(), 0, ofGetWidth(), -12, 12, true),
//                  cos( ofGetElapsedTimef() ) * 1 + 4,
////                  sin( ofGetElapsedTimef() * 1.15 ) * 12
//                  ofMap( ofGetMouseY(), 0, ofGetHeight(), -12, 12, true )
//    );
    
    arm.setTarget( 0.1f,
                  ofMap( ofGetMouseY(), 0, ofGetHeight(), -12, 12, true ),
                  ofMap( ofGetMouseX(), 0, ofGetWidth(), -20, 20, true )
                  );
    
//    arm.setElbowTarget(arm.getTarget().x < 0 ? -7 : 8,
//                       sin( ofGetElapsedTimef() ) * 10,
//                       cos( ofGetElapsedTimef() ) * 10 );
    
//    arm.setElbowTarget( arm.getTarget().x * -0.85,
//                       arm.getTarget().z*1.2,
//                       -12 );
    
    ofVec3f armTarget = -arm.getTarget()+0.1;
//    armTarget.x = -armTarget.x;
//    armTarget.y = 0.01;
//    armTarget *= 0.4;
//    if( arm.getTarget().z < 0.2 ) {
////        armTarget.y = 0.01;
//        armTarget.x = -armTarget.x;
//        armTarget.z = -armTarget.z;
//    } else {
////        armTarget.y =-armTarget.y;
//    }
//        armTarget.z *= -1.f;
//        armTarget.y += 5;
//        armTarget.y *= -1;
//        armTarget.x = 0.01;
    
//    } else {
//        armTarget.y = 0.01;
//    }
//    arm.setElbowTarget( armTarget );
    ofVec3f elbowTarget = arm.getElbowTarget();
    ofVec3f target      = arm.getTarget();
    target.normalize();
    elbowTarget.normalize();
    ofNode tnode;
    ofVec3f cross = target.getCrossed(elbowTarget);
    ofVec3f ucross = cross.getCrossed( ofVec3f(0,0,1 ));
//    tnode.lookAt( elbowTarget, elbowTarget.getCrossed(target) );
//    tnode.lookAt( elbowTarget, ucross );
    ofVec3f tup(0,0,1);
    if( elbowTarget.z < 1 ) {
//        tup.y *= -1;
    }
    tnode.lookAt( elbowTarget, tup );
    
//    if( armTarget.z < 1 ) {
//        arm.setShoulderUpVector( ofVec3f(0,0,1 ));
//    } else {
//        arm.setShoulderUpVector( tnode.getUpDir() );
//    }
    
    
    
    arm.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    glEnable( GL_DEPTH_TEST );
    cam.begin(); {
        ofPushMatrix(); {
            ofSetColor( 180 );
            ofRotateY(-90);
            ofRotateZ(90 );
            ofDrawGridPlane( 4 );
        } ofPopMatrix();
        
        arm.draw();
        ofSetLineWidth( 3 );
        ofSetColor( 80 );
        for( auto& line : lines ) {
            line.draw();
        }
        ofSetLineWidth(1);
    } cam.end();
    glDisable( GL_DEPTH_TEST );
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    if( key == ' ' ) {
        cam.enableMouseInput();
    }
    
    if( key == 'l' ) {
        arm.load("arm.txt");
    }
    
    if( key == 's' ) {
        arm.save("arm.txt");
    }
    
    if( key == OF_KEY_UP ) {
        arm.setUpperArmLength( arm.getUpperArmLength()+1 );
    }
    if( key == OF_KEY_DOWN ) {
        arm.setUpperArmLength( arm.getUpperArmLength()-1 );
    }
    if( key == OF_KEY_LEFT ) {
        arm.setLowerArmLength( arm.getLowerArmLength()-1 );
    }
    if( key == OF_KEY_RIGHT ) {
        arm.setLowerArmLength( arm.getLowerArmLength()+1 );
    }
    if( key == OF_KEY_DEL || key == 127 ) {
        if( lines.size() ) {
            lines.erase( lines.begin() );
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if( key == ' ' ) {
        cam.disableMouseInput();
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
    if( ofGetKeyPressed(' ') ) return;
    mpos.set( arm.getTarget() );
    if( mpos.distance(lastAddPos) > 0.1 ) {
        if( lines.size() ) {
            lines.back().addVertex( mpos );
        }
        lastAddPos = mpos;
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    mpos.set( arm.getTarget() );
    lastAddPos = mpos;
    lines.push_back( ofPolyline() );
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
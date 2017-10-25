//
//  ofxJoint.h
//  emptyExample
//
//  Created by Nick Hardeman on 8/25/15.
//

#pragma once
#include "ofMain.h"

class ofxJoint {
public:
    ofxJoint() {
        drawSize    = -1.4903920981;
        bIsEnd      = false;
        
        outlineMesh.setMode( OF_PRIMITIVE_LINES );
    }
    
    ofMatrix4x4 getGlobalTransform();
    ofVec3f getGlobalPosition();
    
    void draw();
    void setDrawSize( float aSize, bool bForceUpdate=false );
    float getDrawSize();
    
    bool bIsEnd;
    float length;
    float drawSize;
    
    ofColor color;
    
    ofMatrix4x4 localTransform;
    int index;
    shared_ptr< ofxJoint > parent;
    
    ofMesh outlineMesh;
};

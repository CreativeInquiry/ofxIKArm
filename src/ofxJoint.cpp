//
//  ofxJoint.cpp
//  emptyExample
//
//  Created by Nick Hardeman on 8/25/15.
//

#include "ofxJoint.h"

//--------------------------------------------------------------
ofMatrix4x4 ofxJoint::getGlobalTransform() {
    if( parent ) {
        return localTransform * parent->getGlobalTransform();
    }
    return localTransform;
}

//--------------------------------------------------------------
ofVec3f ofxJoint::getGlobalPosition() {
    return getGlobalTransform().getTranslation();
}

//--------------------------------------------------------------
void ofxJoint::draw() {
    
    ofPushMatrix(); {
        ofMultMatrix( getGlobalTransform() );
        
        ofDrawAxis( drawSize );
        
        if( !bIsEnd ) {
            
            ofSetColor( color );
            ofDrawLine( 0,0,0, 0,0,length );
            
            ofSetColor( 255 );
            outlineMesh.draw();
            
        }
    } ofPopMatrix();
}

//--------------------------------------------------------------
void ofxJoint::setDrawSize( float aSize, bool bForceUpdate ) {
    
    if( aSize != drawSize || bForceUpdate ) {
        outlineMesh.clear();
        
        float tlen = aSize;
        float xlen = tlen * 1.5;
        
        ofVec3f p1, p2, p3, p4;
        p1.set( tlen, -tlen, xlen );
        p2.set( -tlen, -tlen, xlen );
        p3.set( -tlen, tlen, xlen );
        p4.set( tlen, tlen, xlen );
        
        outlineMesh.addVertex( p1 ); outlineMesh.addVertex( p2 );
        outlineMesh.addVertex( p2 ); outlineMesh.addVertex( p3 );
        outlineMesh.addVertex( p3 ); outlineMesh.addVertex( p4 );
        outlineMesh.addVertex( p4 ); outlineMesh.addVertex( p1 );
        
        ofVec3f ept( 0, 0, length );
        outlineMesh.addVertex( p1 ); outlineMesh.addVertex( ept );
        outlineMesh.addVertex( p2 ); outlineMesh.addVertex( ept );
        outlineMesh.addVertex( p3 ); outlineMesh.addVertex( ept );
        outlineMesh.addVertex( p4 ); outlineMesh.addVertex( ept );
        
        ept.set( 0,0,0 );
        outlineMesh.addVertex( p1 ); outlineMesh.addVertex( ept );
        outlineMesh.addVertex( p2 ); outlineMesh.addVertex( ept );
        outlineMesh.addVertex( p3 ); outlineMesh.addVertex( ept );
        outlineMesh.addVertex( p4 ); outlineMesh.addVertex( ept );
    }
    
    drawSize = aSize;
}

//--------------------------------------------------------------
float ofxJoint::getDrawSize() { return drawSize; }








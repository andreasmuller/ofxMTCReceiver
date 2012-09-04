//
//  ofxMTCReceiver.cpp
//
//  Created by Andreas Muller on 10/11/2011.
//
//	Based on code by Memo Atken
//

#include "ofxMTCReceiver.h"

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
ofxMTCReceiver::ofxMTCReceiver()
{
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
ofxMTCReceiver::~ofxMTCReceiver()
{
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
void ofxMTCReceiver::init( string _name )
{
	midiIn.listPorts();
	
	int foundPortID = -1;
	for( unsigned int i = 0; i < midiIn.portNames.size(); i++ ) 
	{
		//cout << "|" << _name << "|" << "  |" << midiIn.portNames.at(i) << "|" << endl;
		
		string midiPortName = trim( midiIn.portNames.at(i) );
		
		if( _name == midiPortName )
		{
			cout << "ofxMTCReceiver::init We found port '" << _name << "' at index "<< i << endl; 
			foundPortID = i;
		}
	}
	
	if( foundPortID > -1 )
	{
		init( foundPortID );
	}
	else
	{
		ofLogError() << "ofxMTCReceiver::init could not find " << _name << endl;
	}

}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
void ofxMTCReceiver::init( int _port )
{
	
	midiIn.openPort(_port);	
	midiIn.addListener(this);
	midiIn.setVerbose( false );	
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
void ofxMTCReceiver::newMidiMessage(ofxMidiEventArgs& eventArgs)
{
	//cout << "b1:" << eventArgs.byteOne << " b2:" << eventArgs.byteTwo << " status:" << eventArgs.status<< " port:" << eventArgs.port << " channel: "<< eventArgs.channel  << " timestamp: " << eventArgs.timestamp;
		
	//if( eventArgs == NULL ) return;
	
	ofxMidiEventArgs myEventArgs = eventArgs;
	
	if(myEventArgs.status == 240) {                       // if this is a MTC message...
        // these static variables could be globals, or class properties etc.
        static int times[4]     = {0, 0, 0, 0};                 // this static buffer will hold our 4 time componens (frames, seconds, minutes, hours)
        //static char *szType     = "";                           // SMPTE type as string (24fps, 25fps, 30fps drop-frame, 30fps)
        static int numFrames    = 100;                          // number of frames per second (start off with arbitrary high number until we receive it)
		
        int messageIndex        = myEventArgs.byteOne >> 4;       // the high nibble: which quarter message is this (0...7).
        int value               = myEventArgs.byteOne & 0x0F;     // the low nibble: value
        int timeIndex           = messageIndex>>1;              // which time component (frames, seconds, minutes or hours) is this
        bool bNewFrame          = messageIndex % 4 == 0;
		
		
        // the time encoded in the MTC is 1 frame behind by the time we have received a new frame, so adjust accordingly
        if(bNewFrame) {
            times[kMTCFrames]++;
            if(times[kMTCFrames] >= numFrames) {
                times[kMTCFrames] %= numFrames;
                times[kMTCSeconds]++;
                if(times[kMTCSeconds] >= 60) {
                    times[kMTCSeconds] %= 60;
                    times[kMTCMinutes]++;
                    if(times[kMTCMinutes] >= 60) {
                        times[kMTCMinutes] %= 60;
                        times[kMTCHours]++;
                    }
                }
            }           
            
			//printf("%i:%i:%i:%i | %s\n", times[3], times[2], times[1], times[0], szType);
			
			timcodeEventArgs.hours   = times[3];
			timcodeEventArgs.minutes = times[2];
 			timcodeEventArgs.seconds = times[1];	
			timcodeEventArgs.frames  = times[0];			
			
			timcodeEventArgs.secondFraction = (float)timcodeEventArgs.frames / (float)timcodeEventArgs.numFrames;
			
				
			ofNotifyEvent( MTCEvent, timcodeEventArgs, this );
			
            //sprintf( reportString, "%i:%i:%i:%i | %s\n", times[3], times[2], times[1], times[0], szType );			
        }           
		
		
        if(messageIndex % 2 == 0) {                             // if this is lower nibble of time component
            times[timeIndex]    = value;
        } else {                                                // ... or higher nibble
            times[timeIndex]    |=  value<<4;
        }
		
		
        if(messageIndex == 7) {
            times[kMTCHours] &= 0x1F;                               // only use lower 5 bits for hours (higher bits indicate SMPTE type)
            int smpteType = value >> 1;
            switch(smpteType) {
                case 0: numFrames = 24; /*szType = "24 fps";*/ break;
                case 1: numFrames = 25; /*szType = "25 fps";*/ break;
                case 2: numFrames = 30; /*szType = "30 fps (drop-frame)";*/ break;
                case 3: numFrames = 30; /*szType = "30 fps";*/ break;
                default: numFrames = 100; /*szType = " **** unknown SMPTE type ****";8*/
            }
			
			timcodeEventArgs.numFrames = numFrames;
        }
    }			
}

	
// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
int ofxMTCReceiver::timeToMillis( int _hour, int _minutes, int _seconds, int _millis )
{
	int tmpMillis = 0;
	
	tmpMillis += _hour    * (1000*60*60);
	tmpMillis += _minutes * (1000*60);
	tmpMillis += _seconds * (1000);	
	tmpMillis += _millis;
	
	return tmpMillis;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
string ofxMTCReceiver::timeAsString( int _milliSeconds )
{
	int milliseconds = (int) _milliSeconds % 1000;	
	int seconds = (int) ( _milliSeconds /  1000) % 60 ;
	int minutes = (int) ((_milliSeconds / (1000*60)) % 60);
	int hours   = (int) ((_milliSeconds / (1000*60*60)) % 24);
	
	//cout << ":" << hours << ":" << minutes << ":" << seconds << ":" << milliseconds << endl;
	
	string timeAsString = "";
	if( hours < 10 ) timeAsString = timeAsString + "0";
	timeAsString = timeAsString + ofToString(hours);
	timeAsString = timeAsString + ":";
	
	if( minutes < 10 ) timeAsString = timeAsString + "0";
	timeAsString = timeAsString + ofToString(minutes);	
	timeAsString = timeAsString + ":";
	
	if( seconds < 10 ) timeAsString = timeAsString + "0";
	timeAsString = timeAsString + ofToString(seconds);	
	timeAsString = timeAsString + ":";	
	
	if( milliseconds < 1000 ) timeAsString = timeAsString + "0";
	if( milliseconds < 100 )  timeAsString = timeAsString + "0";
	if( milliseconds < 10 )   timeAsString = timeAsString + "0";
	
	timeAsString = timeAsString + ofToString(milliseconds);	
	
	return timeAsString;
}

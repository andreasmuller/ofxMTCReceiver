//
//  ofxMTCReceiver.h
//
//  Created by Andreas Muller on 10/11/2011.
//
//	Based on code by Memo Atken
//

#pragma once

#include "ofMain.h"
#include "ofxMidi.h"

#define kMTCFrames      0
#define kMTCSeconds     1
#define kMTCMinutes     2
#define kMTCHours       3

//-------------------------------------------------------------------------------------------------------------------------------------
//
class MTCEventArgs : public ofEventArgs 
{
	
public:
	
	MTCEventArgs()
	{
		hours			= 0;
		minutes			= 0;
		seconds			= 0;
		frames			= 0;
		secondFraction  = 0.0f;
		
		numFrames		= 25;
	}
	
	int hours;
	int minutes;
	int seconds;
	int frames;
	float secondFraction;
	
	int numFrames;
};


// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
class ofxMTCReceiver : public ofxMidiListener
{
	public:
		
		ofxMTCReceiver();
		~ofxMTCReceiver();
		
		void init( int _port );
		void init( string _name );
	
		void newMidiMessage(ofxMidiEventArgs& eventArgs);	
		
		static int timeToMillis( int _hour, int _minutes, int _seconds, int _millis );
		static string timeAsString( int _milliSeconds );
	
		ofxMidiIn				midiIn;	
		
		ofEvent<MTCEventArgs>	MTCEvent;
		MTCEventArgs			timcodeEventArgs;
	
	
	
		// trim from both ends
		static inline std::string &trim(std::string &s)  {
			return ltrim(rtrim(s));
		}
		
		// trim from start
		static inline std::string &ltrim(std::string &s) {
			s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
			return s;
		}
		
		// trim from end
		static inline std::string &rtrim(std::string &s)  {
			s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
			return s; 
		}
	
		//char					reportString[512];
};
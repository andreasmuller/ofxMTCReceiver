ofxMTCReceiver
==============

MIDI Timecode receiver addon for openFrameworks.

Believe it or not, sometimes your piece of software will not be running the show and you will have to sync to another time source.

A commonly used source of timecode is MIDI Timecode: http://en.wikipedia.org/wiki/MIDI_timecode

Usage:
------------



    
	string midiDeviceName = "MIDISPORT 2x2 Port A";  
	MTCReceiver.init( midiDeviceName );  

	or:  
  
	int portIndex = 0;  
	MTCReceiver.init( portIndex );  	
  

	// Subscribe to the timecode event  
	ofAddListener( MTCReceiver.MTCEvent, this, &testApp::newMTCMessage  );  


	void testApp::newMTCMessage(MTCEventArgs& _args)
	{  
		// If you need the time as millis  
		int currentMillis = _args.timeAsMillis;
	} 
	

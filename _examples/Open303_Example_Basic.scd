/**
Open303 extension example, with default parameter values, mono-legato note-handling and parameter control by MIDI CC.
toneburst 2025-05-21
*/

//////////////////////////
// Define Open303 Synth //
//////////////////////////

( // SynthDef

s.waitForBoot {
	SynthDef.new("AcidBass", {
		arg out,
		gate        = 0.0,   // Trigger note
		notenum     = 60.0,  // MIDI note number of current note
		notevel     = 64.0,  // MIDI note velocity of current note
		pitchbend   = 0.0,   // pitch-bend (-12 > +12 range)
		waveform    = 0.85,  // Waveform saw > square
		cutoff      = 0.229, // Filter cutoff frequency
		resonance   = 0.5,   // Filter resonance
		envmod      = 0.25,  // Filter envelope-mod amount
		decay       = 0.5,   // Filter envelope decay
		accent      = 0.5,   // Accent amount
		volume      = 0.9,   // Synth output volume
		filtermorph = 0.0,   // Filter-morph 303 -> Bandpass -> Highpass
		pan         = 0.0;   // Output pan L-R (0 = centre)

		//////////////////
		// Declare Vars //
		//////////////////

		var notealloff = NamedControl.tr(\notealloff);
		// Create output
		var sig;

		///////////////////////////
		// Create Open303 Output //
		///////////////////////////

		// Synth. Requires Open303_SuperCollider extension from:
		// https://github.com/toneburst/Open303_SuperCollider/tree/main
		sig = Open303.ar(
			gate:			gate,
			notenum:		notenum,
			notevel:		notevel,
			notealloff:		notealloff,
			pitchbend:      pitchbend,
			waveform:		waveform,
			cutoff:			cutoff,
			resonance:		resonance,
			envmod:			envmod,
			decay:			decay,
			accent:			accent,
			volume:			volume,
			filtermorph:	filtermorph
		);

		//////////////////
		// Final Output //
		//////////////////

		Out.ar(out, Pan2.ar(sig, pan, 1.0));
	}).add;
};

)

( // Start it

/////////////////
// Setup Synth //
/////////////////

// We need to setup a "mono-legato" note-handling system so that the synth can correctly add slides to overlapping notes.
// Create note-stack list. This will contain MIDI note numbers of all currently-held keys
~notestack = List[];

// Create bassline synth instance
~bassline = Synth(\AcidBass);

////////////////////////
// Setup MIDI Control //
////////////////////////

// Start MIDI
MIDIClient.init;
MIDIIn.connectAll;

MIDIFunc.noteOn({ |notevel, notenum|
	// Add new note to note-stack
	~notestack.add(notenum);
	// If note-stack size is now 1, this is a non-legato note. Set slide flag to 0
	if (~notestack.size == 1) {
		// Switch gate high and update synth MIDI note index and velocity. Synth will play note
		~bassline.set(\gate, 1.0, \notenum, notenum, \notevel, notevel);
	} {
		// ...else this is a legato note
		// Hold gate high and update synth note number and velocity. Set slide flag to 1. Synth will slide to new note
		~bassline.set(\gate, 1.0, \notenum, notenum, \notevel, notevel);
	}
});

MIDIFunc.noteOff({ |notevel, notenum|
	// Seach for note index in note-stack and remove
	~notestack.do({ arg item, i; if (item == notenum) { ~notestack.removeAt(i); }});
	// Check if this we've just released the last held note
	if (~notestack.size == 0) {
		// ...we have. Pull gate low and send note index to synth (velocity not required). Synth will release note
		~bassline.set(\gate, 0.0, \notenum, notenum, \notevel, 0.0);
	} {
		// Notes still held. Update synth with most recent note index remaining in note-stack. Synth will slide back to note
		~bassline.set(\gate, 1.0, \notenum, ~notestack.last, \notevel, notevel);
	}
});

MIDIFunc.bend({|val, chan, src|
	~bassline.set(\pitchbend, LinLin(val, 0, 16383, -12.0, 12.0));
});

MIDIFunc.cc({ |val, ccNum, chan, src|
	~bassline.set(\waveform, val / 127);
}, ccNum:21);

MIDIFunc.cc({ |val, ccNum, chan, src|
	~bassline.set(\cutoff, val / 127);
}, ccNum:22);

MIDIFunc.cc({ |val, ccNum, chan, src|
	~bassline.set(\resonance, val / 127);
}, ccNum:23);

MIDIFunc.cc({ |val, ccNum, chan, src|
	~bassline.set(\envmod, val / 127);
}, ccNum:24);

MIDIFunc.cc({ |val, ccNum, chan, src|
	~bassline.set(\decay, val / 127);
}, ccNum:25);

MIDIFunc.cc({ |val, ccNum, chan, src|
	~bassline.set(\accent, val / 127);
}, ccNum:26);

MIDIFunc.cc({ |val, ccNum, chan, src|
	~bassline.set(\filtermorph, val / 127);
}, ccNum:27);

MIDIFunc.cc({ |val, ccNum, chan, src|
	~bassline.set(\volume, val / 127);
}, ccNum:28);

)

// Stop it
~bassline.free;
MIDIdef.freeAll;

/**
Advanced Open303 extension example, demonstrating external input (sub-oscillator and noise).
toneburst 2025-05-21
*/

//////////////////////////
// Define Open303 Synth //
//////////////////////////

( // SynthDef

s.waitForBoot {
	SynthDef.new("AcidBass2", {
		arg out,
		gate        =  0.0,   // Trigger note
		slide       =  0.0,   // Note-slide toggle (0 on non-legato, 1 on legato notes)
		notenum     =  60.0,  // MIDI note number of current note
		notevel     =  64.0,  // MIDI note velocity of current note
		pitchbend   =  0.0,   // pitch-bend (-12 > +12 range)
		waveform    =  0.85,  // Waveform saw > square
		cutoff      =  0.229, // Filter cutoff frequency
		resonance   =  0.5,   // Filter resonance
		envmod      =  0.25,  // Filter envelope-mod amount
		decay       =  0.5,   // Filter envelope decay
		accent      =  0.5,   // Accent amount
		volume      =  1.0,   // Synth output volume
		filtermorph =  0.0,   // Filter-morph 303 -> Bandpass -> Highpass
		extmix      =  0.0,   // Mix oscillator -> external input (pre-filter)
		subnoise    = -1.0,   // External input mix sub-oscillator -> noise (-1 > 1 range)
		pan         =  0.0;   // Output pan L-R (-1 > 1 range)

		//////////////////
		// Declare Vars //
		//////////////////

		var notealloff = NamedControl.tr(\notealloff);
		var slidetime, subcps, subfreq;
		var sig, subosc, noise, extin;

		//////////////////////////////////
		// Process Sub-Oscillator Pitch //
		//////////////////////////////////

		// Sub-oscillator frequency, with lag applied to slide notes.
		// NOTE: Open303 synth handles note-slides internally.
		// Sub-osc may not track the pitch sweep of the Open303 internal oscillator exactly on slide notes!
		slidetime = 0.2 * slide;
		subcps = (notenum - 24).midicps * pitchbend.midiratio;  // Freq. -2 octaves, pitch-bend added
		subfreq = VarLag.kr(subcps, slidetime, -2.0);  // Slewed freq.

		//////////////////////////////////
		// Create External Input Signal //
		//////////////////////////////////

		// Sub-oscillator (-2 octaves squarewave) for external input
		subosc = PulseDPW.ar(subfreq, 0.5);

		// White noise for external input
		//noise = WhiteNoise.ar(1.0);
		// ...or pitch-tracking tuned noise!
		noise = LFNoise0.ar(subfreq * 32.0);

		// Generate external input signal (sub-osc / white noise mix)
		extin = XFade2.ar(subosc, noise, subnoise);

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
			filtermorph:	filtermorph,
			extmix:         extmix,    // External input mix lever
			extin:          extin      // External input
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
// Open303 handles the pitch-sweep on legato notes internally, but we also need to set a "slide" flag on the synth
// as we need to apply slew/lag to the sub-oscillator on slide notes before sending it into the Open303 external input

// Create note-stack list. This will contain MIDI note numbers of all currently-held keys
~notestack = List[];

// Create bassline synth instance
~bassline = Synth(\AcidBass2);

////////////////////////
// Setup MIDI Control //
////////////////////////

// Start MIDI
MIDIClient.init;
MIDIIn.connectAll;

// MIDI functions
MIDIFunc.noteOn({ |notevel, notenum|
	// Add new note to note-stack
	~notestack.add(notenum);
	// If note-stack size is now 1, this is a non-legato note. Set slide flag to 0
	if (~notestack.size == 1) {
		// Switch gate high and update synth MIDI note index and velocity. Synth will play note
		~bassline.set(\gate, 1.0, \notenum, notenum, \notevel, notevel, \subcps, notenum.midicps, \slide, 0.0);
	} {
		// ...else this is a legato note
		// Hold gate high and update synth note number and velocity. Set slide flag to 1. Synth will slide to new note
		~bassline.set(\gate, 1.0, \notenum, notenum, \notevel, notevel, \subcps, notenum.midicps, \slide, 1.0);
	}
});

MIDIFunc.noteOff({ |notevel, notenum|
	// Seach for note index in note-stack and remove
	~notestack.do({ arg item, i; if (item == notenum) { ~notestack.removeAt(i); }});
	// Check if this we've just released the last held note
	if (~notestack.size == 0) {
		// ...we have. Pull gate low and send note index to synth (velocity not required). Synth will release note
		~bassline.set(\gate, 0.0, \notenum, notenum, \notevel, 0.0, \slide, 0.0);
	} {
		// Notes still held. Update synth with most recent note index remaining in note-stack. Synth will slide back to note
		~bassline.set(\gate, 1.0, \notenum, ~notestack.last, \notevel, notevel, \subcps, ~notestack.last.midicps, \slide, 1.0);
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
	~bassline.set(\extmix, val / 127);
}, ccNum:27);

MIDIFunc.cc({ |val, ccNum, chan, src|
	~bassline.set(\subnoise, LinLin.kr(val, 0, 127, -1.0, 1.0));
}, ccNum:28);

)

// Stop it
~bassline.free;
MIDIdef.freeAll;

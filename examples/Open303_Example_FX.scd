/**
Open303 example with added FX.
toneburst 2025-05
*/

//////////////////////////
// Define Open303 Synth //
//////////////////////////

( // SynthDefs

s.waitForBoot {

	// Define Open303 Synth

	SynthDef.new("AcidBass", {
		arg outbus,
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
		filtermorph = 0.0;   // Filter-morph 303 -> Bandpass -> Highpass

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
			filtermorph:    filtermorph
		);

		//////////////////
		// Final Output //
		//////////////////

		Out.ar(outbus, sig);
	}).add;

	// Define FX

	SynthDef.new("AcidFX", {
		arg inbus,
		disttype      = 0,
		distdrive     = 0.5,
		disttone      = 0.5,
		res           = 0.1,
		noise         = 0.0003,
		fxmix         = 1.0,
		pan           = 0,
		out;

		var freq, filtertype, sig, wet;

		// Dry input from bus
		sig = In.ar(inbus, 1);

		////////////////////////////
		// Distortion / Overdrive //
		////////////////////////////

		// Adapted from overdrive and distortion FX by 21echoes:
		// https://github.com/21echoes/pedalboard/tree/master

		// Wet signal. First we feed into a HPF to filter out sub-20Hz
		wet = HPF.ar(sig, 25);

		// ...then we feed into selectable overdrive/distortion
		wet = Select.ar(disttype > 0.5, [
			// Drive controls 1 to 5x the volume with soft-clipping
			(wet * LinLin.kr(distdrive, 0, 1, 1, 5)).softclip,
			// Drive controls 1 to 5x the volume with hard-clipping
			(wet * LinExp.kr(distdrive, 0, 1, 1, 5)).distort
		]);

		// ...then into the Tone section
		// Tone controls a MMF, exponentially ranging from 10 Hz - 21 kHz
		// Tone above 0.75 switches to a HPF
		freq = Select.kr(disttone > 0.75, [
			Select.kr(disttone > 0.2, [
				LinExp.kr(disttone, 0, 0.2, 10, 400),
				LinExp.kr(disttone, 0.2, 0.75, 400, 20000),
			]),
			LinExp.kr(disttone, 0.75, 1, 20, 21000),
		]);

		// ...finally we feed the signal into the filter section
		// Switch filter-type
		filtertype = Select.kr(disttone > 0.75, [0, 1]);
		wet = DFM1.ar(
			in:          wet,
			freq:        freq,
			res:         res,
			inputgain:   1.0,
			type:        filtertype,
			noiselevel:  noise
		).softclip;

		// Naive level-compensation
		wet = wet * LinLin.kr(distdrive, 0, 1, 0.75, 0.5);

		// Wet/dry mix distorted and dry signals
		sig = XFade2.ar(sig, wet, fxmix);

		//////////////////
		// Final Output //
		//////////////////

		Out.ar(out, Pan2.ar(sig, pan));
	}).add;
};

)

( // Start it

//////////////
// FX Setup //
//////////////

// Create FX bus (mono audio)
~fxbus = Bus.audio(s, 1);

/////////////////
// Setup Synth //
/////////////////

// Create bassline synth instanceand set output bus
~bassline = Synth(\AcidBass);
~bassline.set(\outbus, ~fxbus);

// Create FX instance, adding after ~bassline and set input bus
~fx = Synth.after(~bassline, \AcidFX);
~fx.set(\inbus, ~fxbus, \out, 1);

////////////////////////
// Setup MIDI Control //
////////////////////////

// We need to setup a "mono-legato" note-handling system so that the synth can correctly add slides to overlapping notes.
// Create note-stack list. This will contain MIDI note numbers of all currently-held keys
~notestack = List[];

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
	~bassline.set(\pitchbend, linlin(val, 0, 16383, -12.0, 12.0));
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
	~fx.set(\fxmix, LinLin.kr(val, 0, 127, -1.0, 1.0));
}, ccNum:26);

MIDIFunc.cc({ |val, ccNum, chan, src|
	~fx.set(\distdrive, val / 127);
}, ccNum:27);

MIDIFunc.cc({ |val, ccNum, chan, src|
	~fx.set(\disttone, val / 127);
}, ccNum:28);

)

// Stop it
~bassline.free;
MIDIdef.freeAll;

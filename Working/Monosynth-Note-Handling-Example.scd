( // SynthDef
s.waitForBoot {
  SynthDef(\monosynth, { |out, freq = 440, amp, gate, bend, pan, portamento = 0.1,
                          atk = 0.1, dec = 0.3, sus = 0.5, rel = 0.5|
    var env = Env.adsr(atk, dec, sus, rel).ar(gate: gate);
    var sig = SinOsc.ar(freq.lag2(portamento) * bend.midiratio);
    Out.ar(out, Pan2.ar(sig, pan, amp) * env);
  }).add;
};
)


(// start it
~noteStack = [];
~synth = Synth(\monosynth);

MIDIClient.init;
MIDIIn.connectAll;

MIDIdef.noteOn(\noteOn, { |vel, num|
  var freq = num.midicps;
  var amp = vel.linexp(0, 127, 0.05, 1);
  ~synth.set(\freq, freq, \amp, amp, \gate, 1);
  ~noteStack = ~noteStack.add(num);
});
MIDIdef.noteOff(\noteOff, { |vel, num|
  ~noteStack.remove(num);
  if (~noteStack.size > 0) {
    ~synth.set(\freq, ~noteStack.last.midicps);
  } {
    ~synth.set(\gate, 0)
  };
});
MIDIdef.bend(\bend, { |val|
  var bendSemitones = 12;
  ~synth.set(\bend, (val / 8192.0 - 1) * bendSemitones);
});
)



// stop it
~synth.free;
MIDIdef.freeAll;
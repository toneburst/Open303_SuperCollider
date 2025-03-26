// Conversion functions from GlobalFunctions.h in JavaScript
// Run in JSFiddle or similar
// https://jsfiddle.net

function linToLin(val, inMin, inMax, outMin, outMax) {
    // map input to the range 0.0...1.0:
    var tmp = (val-inMin) / (inMax-inMin);
  
    // map the tmp-value to the range outMin...outMax:
    tmp *= (outMax-outMin);
    tmp += outMin;
  
    return tmp;
  }
  
  function expToLin(val, inMin, inMax, outMin, outMax) {
    tmp = Math.log(val/inMin) / Math.log(inMax/inMin);
    return outMin + tmp * (outMax-outMin);
  }
  
  function expToLin(val, inMin, inMax, outMin, outMax) {
    tmp = Math.log(val/inMin) / Math.log(inMax/inMin);
    return outMin + tmp * (outMax-outMin);
  }
  
  var result =  linToLin(  -6.0,  -60.0,     0.0, 0.0,  1.0);
  
  console.log(result);

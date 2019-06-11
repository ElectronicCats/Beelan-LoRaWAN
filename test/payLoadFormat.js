// function to payload in the things network
function Decoder(bytes, port) {
    // Decode an uplink message from a buffer
    // (array) of bytes to an object of fields.
    var decoded = {};
  
    // if (port === 1) decoded.led = bytes[0];
    var chan    = bytes[0];
    var rate    = bytes[1];
    var txP     = bytes[2];
    var tr      = bytes[3];
    var cntint  = (bytes[4] << 8) | bytes[5];
    
    decoded.chan = chan;
    decoded.rate = rate;
    decoded.Power = txP; 
    decoded.tried = tr;
    decoded.cnt = cntint;
    
    return decoded;
}
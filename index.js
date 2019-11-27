var mosca = require('mosca');
require('./pub.js');
require('./sub.js');

var settings={
    port:1883,
    http:{
        port:1884,
        bundle: true,
        static:'./'
    }

}
var server = new mosca.Server(settings);
server.on('ready',function() {
    console.log("redo!");
})
server.on('clientConnected',function(client) {
    console.log("Client connected", client.id);
})
server.on('published',function(packet) {
    // console.log(packet);
    // console.log("Published message"+ packet.payload+" on "+ packet.topic);
})
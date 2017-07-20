var index_0 = Module.cwrap('get_index_0', 'bool', []);
var index_1 = Module.cwrap('get_index_1', 'bool', []);
var index_2 = Module.cwrap('get_index_2', 'bool', []);
var index_3 = Module.cwrap('get_index_3', 'bool', []);

function graph(pos, value) {
	var graphCTX = document.getElementById("grapharea");
	index_0();
	index_1();
	index_2();
	index_3();
}

var ws = new WebSocket("ws://"+window.location.hostname+":8888/socket");
ws.onopen = function() {
	ws.send("Hello Server");
};

var queue_push = Module.cwrap('queue_push', 'null', ['string']);

ws.onmessage = function (evt) {
	queue_push(evt.data);
};

setInterval(graph, 10)

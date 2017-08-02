var graphCTX = document.getElementById("grapharea");
var graphcont = document.getElementById("graphcontainer");

var pos = 1000;

/*
 * Ze Magic Constants
 * DO NOT TOUCH!
 * Based on trial and error, vodoo magic and some limited logic.  
 * 
 * Yes, I know it breaks on IOS devices and some Androids in portrait mode.
 * But this was built for TVs
 */
const GRAPH_HEIGHT = graphcont.offsetHeight;
const LINE_WIDTH = (GRAPH_HEIGHT/99).toString();
const GRAPH_LINES = [
	((GRAPH_HEIGHT/5)-(GRAPH_HEIGHT/17)).toString(),
	(((2*GRAPH_HEIGHT)/5)-(GRAPH_HEIGHT/35)).toString(),
	(((3*GRAPH_HEIGHT)/5)+(GRAPH_HEIGHT/90)).toString(),
	(((4*GRAPH_HEIGHT)/5)+(GRAPH_HEIGHT/27)).toString()
];
const LINE_LENGTH = graphcont.offsetWidth;



const LINE_COLORS = [
	'#f00',
	'#ff0',
	'#0f0',
	'#00f',
]

var state = [
	false,
	false,
	false,
	false
];

var index = [
	Module.cwrap('get_index_0', 'number', []),
	Module.cwrap('get_index_1', 'number', []),
	Module.cwrap('get_index_2', 'number', []),
	Module.cwrap('get_index_3', 'number', [])
]

console.log();

function graph() {
	for (var i = 0; i < 4; i++){
		if (index[i] === 1) {
		} else {
			var newElement = document.createElementNS("http://www.w3.org/2000/svg", 'path');
			newElement.setAttribute("d","M 0 "+GRAPH_LINES[i]+" L "+ LINE_LENGTH +" "+GRAPH_LINES[i]);
			newElement.style.stroke = LINE_COLORS[i];
			newElement.style.strokeWidth = (LINE_WIDTH+"px");
			graphCTX.appendChild(newElement);
		}
	}
}

var ws = new WebSocket("ws://"+window.location.hostname+":8888/socket");
ws.onopen = function() {
	ws.send("Hello Server");
};

var queue_push = Module.cwrap('queue_push', 'null', ['string']);

ws.onmessage = function (evt) {
	queue_push(evt.data);
};

setInterval(graph, 10);

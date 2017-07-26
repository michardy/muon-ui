var graphCTX = document.getElementById("grapharea");
var graphcont = document.getElementById("graphcontainer");

var pos = 1000;

/*
 * Ze Magic Constants
 * DO NOT TOUCH
 */
const GRAPH_HEIGHT = graphcont.offsetHeight;
const LINE_WIDTH = (GRAPH_HEIGHT/99).toString();
const GRAPH_LINES = [
	((GRAPH_HEIGHT/4)-(GRAPH_HEIGHT/17)).toString(),
	(((2*GRAPH_HEIGHT)/4)-(LINE_WIDTH/2)).toString(),
	(((3*GRAPH_HEIGHT)/4)+(GRAPH_HEIGHT/35)).toString(),
];
const LINE_LENGTH = graphcont.offsetWidth;


var state_0 = false;
var state_1 = false;
var state_2 = false;
var state_3 = false;

var index_0 = Module.cwrap('get_index_0', 'bool', []);
var index_1 = Module.cwrap('get_index_1', 'bool', []);
var index_2 = Module.cwrap('get_index_2', 'bool', []);
var index_3 = Module.cwrap('get_index_3', 'bool', []);

console.log();

function graph() {//index_0, index_1, index_2, index_3) {
	if (index_0()) {
		
	} else {
		var newElement = document.createElementNS("http://www.w3.org/2000/svg", 'path');
		newElement.setAttribute("d","M 0 "+GRAPH_LINES[0]+" L "+ LINE_LENGTH +" "+GRAPH_LINES[0]);
		newElement.style.stroke = "#f00";
		newElement.style.strokeWidth = (LINE_WIDTH+"px");
		graphCTX.appendChild(newElement);
	}
	if (index_1()) {
		
	} else {
		var newElement = document.createElementNS("http://www.w3.org/2000/svg", 'path');
		newElement.setAttribute("d","M 0 "+GRAPH_LINES[1]+" L "+ LINE_LENGTH +" "+GRAPH_LINES[1]);
		newElement.style.stroke = "#0f0";
		newElement.style.strokeWidth = (LINE_WIDTH+"px");
		graphCTX.appendChild(newElement);
	}
	if (index_2()) {
		
	} else {
		var newElement = document.createElementNS("http://www.w3.org/2000/svg", 'path');
		newElement.setAttribute("d","M 0 "+GRAPH_LINES[2]+" L "+ LINE_LENGTH +" "+GRAPH_LINES[2]);
		newElement.style.stroke = "#00f";
		newElement.style.strokeWidth = (LINE_WIDTH+"px");
		graphCTX.appendChild(newElement);
	}
	if (index_3()) {
		
	} else {
		/*var newElement = document.createElementNS("http://www.w3.org/2000/svg", 'path');
		newElement.setAttribute("d","M 0 "+GRAPH_LINES[2]+" L "+ LINE_LENGTH +" "+GRAPH_LINES[0]);
		newElement.style.stroke = "#00f";
		newElement.style.strokeWidth = (LINE_WIDTH+"px");
		graphCTX.appendChild(newElement);*/
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

setInterval(graph, 10);//, index_0, index_1, index_2, index_3)

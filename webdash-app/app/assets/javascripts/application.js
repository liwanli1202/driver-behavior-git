// This is a manifest file that'll be compiled into application.js, which will include all the files
// listed below.
//
// Any JavaScript/Coffee file within this directory, lib/assets/javascripts, vendor/assets/javascripts,
// or vendor/assets/javascripts of plugins, if any, can be referenced here using a relative path.
//
// It's not advisable to add code directly here, but if you do, it'll appear at the bottom of the
// compiled file.
//
// Read Sprockets README (https://github.com/sstephenson/sprockets#sprockets-directives) for details
// about supported directives.
//
//= require jquery
//= require jquery_ujs
//= require jquery.flot
//= require jquery.flot.resize
//= require jquery.flot.crosshair
//= require jquery.flot.categories
//= require turbolinks
//= require bootstrap.min
//= require_tree .


$(document).ready(function() {
	var websocket = null;
	
	// WebSocket versions and backwards compatibility
	if ('WebSocket' in window) {
		websocket = new WebSocket('ws://0.0.0.0:8126');
	}
	else if ('MozWebSocket' in window) {
		websocket = new MozWebSocket('ws://0.0.0.0:8126');
	}
	else {
		return;
	}
	
	websocket.onopen = function() {
		console.log("Socket opened");
	};
	
	websocket.onmessage = function(message) {
		//console.log(message);
		//console.log(message.data);
		//$('#echo').append(message.data+"<br>");
		setData(message.data);
		update();
		/*var comment = eval('(' + message.data + ')');
		console.log(comment);
		console.log(comment["article_id"]);
		if (window.location.pathname == "/articles/" + comment["article_id"]) {
			var commentHtml = "<div class=\"comment\">" +"<em> on " + comment["pretty_date"] + "</em>" +"<p>" + comment["contents"] + "</p>";
			console.log(commentHtml);
			$('#comments').prepend(commentHtml);
		}*/
	};
	
	websocket.onclose = function() {
		console.log("Socket closed");
	};
	
	
	//
	// Below: Drawing real time plots
	//
	var data = [], totalPoints = 300;
	var bardata = [0,0,0,0,0,0,0,0];
	//bardata array
	// bardata[0] = last stability
	// bardata[1] = avg. stability
	// bardata[2] = avg. lane change freq
	// bardata[3] = last lane change speed
	// bardata[4] = avg. lane change speed
	// bardata[5] = total stab count
	// bardata[6] = total freq count
	// bardata[7] = total speed count

  setInitialData();
	
	function setInitialData()
	{
		if(data.length>0)
		  data = data.slice(1);
		
		while (data.length < totalPoints) 
		{
				data.push(0);
		}
	}
	
	
	function setData(literal)
	{
		//console.log("literal: "+literal);
		
		//frame data format
		// f,framenumber,timestamp,lateralPosition,Yaw
		// f,518,30645,0.240599,5.95347e-08

		var rand = Math.floor((Math.random() * 10) + 1);
		var params = literal.split(",");
		if(params[0]=="f")
		{
			var lpos = params[3];
			data.shift();
   		data.push(lpos);
			setBarData(lpos*10,rand,rand);
		}		
		

		//console.log("Frame"+frame_number[1]+" lpos:"+lpos);
	}
	
	function setBarData(stab,freq,speed)
	{
	  var avg_stab = ((bardata[1] * bardata[5])+stab)/(bardata[5]+1);
	  var avg_freq = ((bardata[2] * bardata[6])+freq)/(bardata[6]+1);
 	  var avg_speed = ((bardata[4] * bardata[7])+freq)/(bardata[7]+1);
 	  
		bardata[0] = stab;
		bardata[1] = avg_stab;
		bardata[2] = avg_freq;
		bardata[3] = speed;
		bardata[4] = avg_speed;
		bardata[5] = bardata[5]+1;
		bardata[6] = bardata[6]+1;
		bardata[7] = bardata[7]+1;
	}
	
	function getBarData()
	{
		var temp_dataset = [ ["Stability", bardata[0]], ["Avg Stability", bardata[1]], ["Avg Lane change freq", bardata[2]], ["Lane change Speed", bardata[3]], ["Avg Lane change speed", bardata[4]] ];
		
		return temp_dataset;
	}
	
	
	function getData()
	{
			
			var res = [];
			for (var i = 0; i < data.length; ++i) 
			{
				res.push([i, data[i]]);
			}

			return res;
			
	}
	
  
	function getRandomData() {

			if (data.length > 0)
				data = data.slice(1);

			// Do a random walk

			while (data.length < totalPoints) {

				var prev = data.length > 0 ? data[data.length - 1] : 50,
					y = prev + Math.random() * 10 - 5;

				if (y < 0) {
					y = 0;
				} else if (y > 100) {
					y = 100;
				}

				data.push(y);
			}

			// Zip the generated y values with the x values

			var res = [];
			for (var i = 0; i < data.length; ++i) {
				res.push([i, data[i]])
			}

			return res;
			
	}
  
	// Set up the control widget

	var updateInterval = 30;

	var plot = $.plot("#placeholder", [ getData() ], {
		series: {
			shadowSize: 0	// Drawing is faster without shadows
		},
		crosshair: {
			mode: "xy"
		},
		grid: {
			hoverable: true,
			autoHighlight: false,
			markings: [
			{ color: "#818181", yaxis: { from: 2 } },
			{ color: "#818181", yaxis: { to: -2 } }
      ]
		},
		yaxis: {
			min: -5,
			max: 5
		},
		xaxis: {
			
			ticks: [],
			autoscaleMargin: 0.02
		}
	});

  var ndata = [ ["Stability", bardata[0]], ["Avg Stability", bardata[1]], ["Avg Lane change freq", bardata[2]], ["Lane change Speed", bardata[3]], ["Avg Lane change speed", bardata[4]] ];

	var bar = $.plot("#barholder", [{ data: ndata, color: "#298ACC" }], {
		series: {
			// Drawing is faster without shadows
			bars: {
				show: true,
				barWidth: 0.6,
				fill: 0.7,
				align: "center"
			}
		},
		crosshair: {
			mode: "y"
		},
		grid: {
			hoverable: true,
			autoHighlight: false
		},
		yaxis: {
			min: 0,
			max: 10
		},
		xaxis: {
			mode: "categories",
			tickLength: 0
		}
	});

	function update() {
		//console.log("update started");
		plot.setData([getData()]);
		bar.setData([getBarData()]);
		//console.log("random data obtained. Draw started");
		// Since the axes don't change, we don't need to call plot.setupGrid()
		bar.draw();
		plot.draw();
		//console.log("plot redrawn");
		//setTimeout(update, updateInterval);
	}

	//update();
	
});

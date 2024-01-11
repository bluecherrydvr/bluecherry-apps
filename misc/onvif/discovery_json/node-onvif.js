#!/usr/bin/env node
/**
 * Discover ONVIF devices on the network
 *
 * Created by Roger Hardiman <opensource@rjh.org.uk>
 * Adapted by Andriy Utkin at Bluecherry
 */

var onvif = require('onvif');
var xml2js = require('xml2js')
var stripPrefix = require('xml2js').processors.stripPrefix;

onvif.Discovery.on('device', function(cam, rinfo, xml) {
	// Function will be called as soon as the NVT responses


	// Parsing of Discovery responses taken from my ONVIF-Audit project, part of the 2018 ONVIF Open Source Challenge
	// Filter out xml name spaces
	xml = xml.replace(/xmlns([^=]*?)=(".*?")/g, '');

	let parser = new xml2js.Parser({
		attrkey: 'attr',
		charkey: 'payload',                // this ensures the payload is called .payload regardless of whether the XML Tags have Attributes or not
		explicitCharkey: true,
		tagNameProcessors: [stripPrefix]   // strip namespace eg tt:Data -> Data
	});
	parser.parseString(xml,
		function(err, result) {
			if (err) {return;}
			let urn = result['Envelope']['Body'][0]['ProbeMatches'][0]['ProbeMatch'][0]['EndpointReference'][0]['Address'][0].payload;
			let xaddrs = result['Envelope']['Body'][0]['ProbeMatches'][0]['ProbeMatch'][0]['XAddrs'][0].payload;
			let scopes = result['Envelope']['Body'][0]['ProbeMatches'][0]['ProbeMatch'][0]['Scopes'][0].payload;
			scopes = scopes.split(" ");

			let hardware = "";
			let name = "";
			for (let i = 0; i < scopes.length; i++) {
				if (scopes[i].includes('onvif://www.onvif.org/name')) {name = decodeURI(scopes[i].substring(27));}
				if (scopes[i].includes('onvif://www.onvif.org/hardware')) {hardware = decodeURI(scopes[i].substring(31));}
			}
			//let msg = 'Discovery Reply from ' + rinfo.address + ' (' + name + ') (' + hardware + ') (' + xaddrs + ') (' + urn + ')';
			// Hikvision cameras put multiple URLs into the same field.
			// Let's take just the first one.
			// console.log(result['Envelope']['Body'][0]['ProbeMatches'][0]['ProbeMatch'][0]['XAddrs']);
			// ... payload: 'http://192.168.86.200/onvif/device_service http://[fe80::2a57:beff:fecb:cca6]/onvif/device_service'
			// ... payload: 'http://192.168.86.109/onvif/device_service http://[fe80::a614:37ff:fed8:5c49]/onvif/device_service'

			url = xaddrs.split(' ')[0];
			urlobj = new URL(url);
			port = urlobj.port ? urlobj.port : '80';
			host_port = urlobj.host + ':' + port;
			msg = {
				'ipv4': rinfo.address,
				'manufacturer': name,
				'model_name': hardware,
				'ipv4_path': url,
				'ipv4_port': host_port,
			};
			console.log(JSON.stringify(msg));
		}
	);
})
onvif.Discovery.on('error', function(err,xml) {
	// The ONVIF library had problems parsing some XML
	process.stderr.write('Discovery error ' + err);
	process.exit(1);
});
onvif.Discovery.probe();

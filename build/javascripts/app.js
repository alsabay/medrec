// Import the page's CSS. Webpack will know what to do with it.
import "../stylesheets/app.css";
// Import js libraries
/*
import "../javascripts/bootstrap.min.js";
import "../javascripts/bootstrap.js";
import "../javascripts/jquery-1.8.1.min.js";
import "../javascripts/jsbn/jsbn2.js";
import "../javascripts/jsbn/prng4.js";
import "../javascripts/jsbn/rng.js";
import "../javascripts/jsbn/jsbn.js";
import "../javascripts/paillier.js";
*/
// Import libraries we need.
import { default as Web3} from 'web3';
import { default as contract } from 'truffle-contract'

// Import our contract artifacts and turn them into usable abstractions.
import medical_record_artifacts from '../../build/contracts/Medical_record.json'

var Medical_record = contract(medical_record_artifacts);

// The following code is simple to show off interacting with your contracts.
// As your needs grow you will likely need to change its form and structure.
// For application bootstrapping, check out window.addEventListener below.

var prior_vitals = {"Pulse": "pulse_pv", "Respiration": "resp_pv", "Temperature": "temp_pv", "BP-sys":"bps_pv", "BP-dias": "bpd_pv", "Weight": "weight_pv", "Height-ft": "htft_pv", "Height-in": "htin_pv"}
var cur_vitals = {"Pulse": "pulse_nv", "Respiration": "resp_nv", "Temperature": "temp_nv", "BP-sys":"bps_nv", "BP-dias": "bpd_nv", "Weight": "weight_nv", "Height-ft": "htft_nv", "Height-in": "htin_nv"}
var diff_vitals = {"Pulse": "pulse_ch", "Respiration": "resp_ch", "Temperature": "temp_ch", "BP-sys":"bps_ch", "BP-dias": "bpd_ch", "Weight": "weight_ch", "Height-ft": "htft_ch", "Height-in": "htin_ch"}
var input_vitals = {"Pulse": "pulse_val", "Respiration": "resp_val", "Temperature": "temp_val", "BP-sys":"bpsys_val", "BP-dias": "bpdias_val", "Weight": "weight_val", "Height-ft": "htft_val", "Height-in": "htin_val"}
var dec_vitals = {"Pulse": "pulse_dec", "Respiration": "resp_dec", "Temperature": "temp_dec", "BP-sys":"bps_dec", "BP-dias": "bpd_dec", "Weight": "weight_dec", "Height-ft": "hft_dec", "Height-in": "hin_dec"}
var enc_vitals = {"Pulse": "pulse_en", "Respiration": "resp_en", "Temperature": "temp_en", "BP-sys":"bps_en", "BP-dias": "bpd_en", "Weight": "weight_en", "Height-ft": "hft_en", "Height-in": "hin_en"}

var v_array = new Array();
var curr = new Array();
var prio = new Array();

function readInput(){
    var vnames = Object.keys(input_vitals);
    for (var i = 0; i< vnames.length; i++){
        let in_vital = vnames[i];
        v_array[i] = $('#' + input_vitals[in_vital]).val();
    }
    console.log(v_array);
    return v_array;
}


window.setRecord = function(){
    var inp = readInput();
    try{
        $("#msg").html("New Medical Vitals submitted. The Display will update when the transaction has been recorded on the blockchain. Please wait....")
        Medical_record.deployed().then(function(contractInstance){
            contractInstance.updatenewVitals(inp, {from: web3.eth.accounts[0]}).then(function(){
                $("#msg").html("New Vitals Successfully Recorded in Ethereum Blockchain..."); 
                updatePage();
            });
        }); 
    }catch (err) {
        console.log(err);
    } 
}


function updatePage(){
    console.log("Updating Page");
    Medical_record.deployed().then(function(contractInstance){
        contractInstance.getRecord.call('current',{from: web3.eth.accounts[0]}).then(function(v){
            console.log("Updating Current");
            let vnames = Object.keys(cur_vitals);
            for (var i = 0; i < vnames.length; i++){
                curr[i] = parseInt(v[i].toString());
                let vital = vnames[i];
                $("#" + cur_vitals[vital]).html(v[i].toString());
                $("#" + dec_vitals[vital]).html(v[i].toString());
                
            }
        });
        contractInstance.getRecord.call('prior',{from: web3.eth.accounts[0]}).then(function(v){
            console.log("Updating Prior");
            let vnames = Object.keys(prior_vitals);
            for (var i = 0; i < vnames.length; i++){
                prio[i] = parseInt(v[i].toString());
                let vital = vnames[i];
                $("#" + prior_vitals[vital]).html(v[i].toString());
                $("#" + diff_vitals[vital]).html(curr[i] - v[i]);
            }
        });
    });
}

$(document).ready(function() {
    if (typeof web3 !== 'undefined') {
        console.warn("Using web3 detected from external source like Metamask")
                //Use Mist/MetaMask's provider
        window.web3 = new Web3(web3.currentProvider);
    } else {
        console.warn("No web3 detected. Falling back to http://localhost:8545. You should remove this fallback when you deploy live, as it's inherently insecure. Consider switching to Metamask for development. More info here: http://truffleframework.com/tutorials/truffle-and-metamask");
                // fallback - use your fallback strategy (local node / hosted node + in-dapp id mgmt  fail)
        window.web3 = new Web3(new Web3.providers.HttpProvider("http://localhost:8545"));
    }

    Medical_record.setProvider(web3.currentProvider);
    updatePage();
});


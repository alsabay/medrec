pragma solidity ^0.4.18;

contract Medical_record {
/* create data structure for patient vitals */

struct Vitals {
    uint pulse;
    uint resprate;
    uint bodytemp;
    uint bpsystolic;
    uint bpdiastolic;
    uint weight;
    uint heightft;
    uint heightin;
}

mapping(uint => Vitals) public vrecords;
uint public recIndex;

/* Constructor. This is called only once during blockchain deployment to 
create a new medical record contract of a person
*/
function Medical_record() public{
    recIndex = 0;
}

function updatenewVitals(uint[] newvitals) public{

    vrecords[recIndex].pulse = newvitals[0];
    vrecords[recIndex].resprate = newvitals[1];
    vrecords[recIndex].bodytemp = newvitals[2];
    vrecords[recIndex].bpsystolic = newvitals[3];
    vrecords[recIndex].bpdiastolic = newvitals[4];
    vrecords[recIndex].weight = newvitals[5];
    vrecords[recIndex].heightft = newvitals[6];
    vrecords[recIndex].heightin = newvitals[7];
    recIndex += 1;

}
// if idx = 'current' it will return current record, if idx = 'prior' it will return prior
// record.  
function getRecord(bytes32 idx) constant public returns (
    uint pul,
    uint res,
    uint tem,
    uint bps,
    uint bpd,
    uint wgt,
    uint hft,
    uint hin
){

    uint offset;
    require(idx == 'current' || idx == 'prior');
    if (idx == 'current'){
        offset = 1;
    }else if (idx == 'prior'){
        offset = 2;
    }
    uint index = (recIndex - offset);
    require(index >= 0);
    pul = vrecords[index].pulse;
    res = vrecords[index].resprate;
    tem = vrecords[index].bodytemp;
    bps = vrecords[index].bpsystolic;
    bpd = vrecords[index].bpdiastolic;
    wgt = vrecords[index].weight;
    hft = vrecords[index].heightft;
    hin = vrecords[index].heightin;

}

}

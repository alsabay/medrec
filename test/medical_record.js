var MedicalRecord = artifacts.require('./Medical_record.sol');

contract('Medical_record:getCurrentVitals()', function(accounts) {
  it("should assert true", function(done) {
    var medical_record = MedicalRecord.deployed();
    medical_record.then(function(records){
        return records.getCurrentVitals.call();
    }).then(function(result){
        //print the result

    })
 });
});

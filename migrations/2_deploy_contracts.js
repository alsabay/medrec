var Medical_record = artifacts.require("./Medical_record.sol");

module.exports = function(deployer) {
  deployer.deploy(Medical_record, {gas: 4700000} );
};

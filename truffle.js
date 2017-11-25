// Allows us to use ES6 in our migrations and tests.
require('babel-register')

module.exports = {
  networks: {
    development: {
      host: 'localhost',
      port: 8545,
      network_id: '*', // Match any network id
      gas: 4700036,
      from: '0x527189490ba9cd3762aa63403ff425c3d90ed26f'    
    }
  }
}

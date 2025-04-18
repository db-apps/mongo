/**
 * This test checks the upgrade path from noauth to keyFile.
 *
 * This test requires users to persist across a restart.
 * @tags: [requires_persistence, requires_replication]
 */
import "jstests/multiVersion/libs/multi_rs.js";

import {ReplSetTest} from "jstests/libs/replsettest.js";

// We turn off gossiping the mongo shell's clusterTime because this test connects to replica sets
// and sharded clusters as a user other than __system. Attempting to advance the clusterTime while
// it has been signed with a dummy key results in an authorization error.
TestData.skipGossipingClusterTime = true;
TestData.skipCheckOrphans = true;

var keyFilePath = 'jstests/libs/key1';

// Disable auth explicitly
var noAuthOptions = {noauth: ''};

// Undefine the flags we're replacing, otherwise upgradeSet will keep old values.
var transitionToAuthOptions =
    {noauth: undefined, clusterAuthMode: 'keyFile', keyFile: keyFilePath, transitionToAuth: ''};
var keyFileOptions = {
    clusterAuthMode: 'keyFile',
    keyFile: keyFilePath,
    transitionToAuth: undefined
};

var rst = new ReplSetTest({name: 'noauthSet', nodes: 3, nodeOptions: noAuthOptions});
rst.startSet();
rst.initiate(null, null, {initiateWithDefaultElectionTimeout: true});

var rstConn1 = rst.getPrimary();

// Create a user to login as when auth is enabled later
rstConn1.getDB('admin').createUser({user: 'root', pwd: 'root', roles: ['root']});

rstConn1.getDB('test').a.insert({a: 1, str: 'TESTTESTTEST'});
assert.eq(1, rstConn1.getDB('test').a.count(), 'Error interacting with replSet');

print('=== UPGRADE noauth -> transitionToAuth/keyFile ===');
rst.upgradeSet(transitionToAuthOptions);
var rstConn2 = rst.getPrimary();
rstConn2.getDB('test').a.insert({a: 1, str: 'TESTTESTTEST'});
assert.eq(2, rstConn2.getDB('test').a.count(), 'Error interacting with replSet');

print('=== UPGRADE transitionToAuth/keyFile -> keyFile ===');
rst.upgradeSet(keyFileOptions, 'root', 'root');

// upgradeSet leaves its connections logged in as root
var rstConn3 = rst.getPrimary();
rstConn3.getDB('test').a.insert({a: 1, str: 'TESTTESTTEST'});
assert.eq(3, rstConn3.getDB('test').a.count(), 'Error interacting with replSet');

rst.stopSet();

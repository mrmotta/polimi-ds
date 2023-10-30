# Messages

This is meant to be a comprehensive list of all the possible messages exchanged at runtime between the various components of the system.

---

## `PUT`

Inserts value `v` with key `k` in datastore.

**From**: client

**To**: replica

### Arguments

- `k`: key

- `v`: value

### Possible Responses

- `ACK`: if everything went as planned

- `NACK`: otherwise

---

## `GET`

Queries value for key `k`.

**From**: client

**To**: replica

### Arguments

- `k`: key

### Possible responses

- `VALUE`: if everything went as expected and the key is present, the system returns the stored value

- `NACK`: otherwise

---

## `FETCH`

Queries value for key `k`.

**From**: (coordinating) replica

**To**: (multiple) replica(s)

### Arguments

- `k`: key

### Possible responses

- `FETCHED_VALUE` (see later): the only possible response (in case of a writing lock, it waits for the value to be available)

---

## `FETCHED_VALUE`

Response of a `FETCH` request.

**From**: replica

**To**: coordinating replica

### Arguments

- `k`: requested key

- `v`: fetched value

- `ts`: timestamp of the retrieved value

### Possible responses

No required response

---

## `PREPARE`

Initiates three-phase commit on key `k`.

**From**: coordinating replica

**To**: multiple replicas

### Arguments

- `k`: key

- `v`: value

### Possible responses

- `VOTE_COMMIT` (see later): if the replica successfully put a lock on the key

- `VOTE_ABORT` (see later): if there is already a lock taken on that resource

---

## `VOTE_COMMIT`

Tells the coordinator that the request has been accepted.

**From**: replica

**To**: coordinating replica

### Arguments

- `k`: key on which the lock has been granted (the same of the requested one)

- `ts`: timestamp associated to the value according to this replica (which is the current one with respect to that key)

### Possible responses

- `GLOBAL_COMMIT` (see later): if all the contacted replicas replied with `VOTE_COMMIT` by a given time

- `GLOBAL_ABORT` (see later): otherwise

---

## `VOTE_ABORT`

Tells the coordinator that the request has been rejected (because of a writing lock).

**From**: replica

**To**: coordinating replica

### Arguments

- `k`: key of the value that should have been set

### Possible responses

- `GLOBAL_ABORT` (see later): which is automatically discarded since no operation has been performed on the key-value pair of this replica

---

## `GLOBAL_ABORT`

Tells the replicas to abort the operation, restoring the previous value.

**From**: coordinating replica

**To**: replicas

### Arguments

- `k`: key for which the write operation has to be aborted

### Possible responses

- `ACK`

---

## `GLOBAL_COMMIT`

Tells the replicas to actually commit the value after all the `VOTE_COMMIT` messages have been received.

**From**: coordinating replica

**To**: multiple replicas

### Arguments

- `k`: key on which the commit operation has to be performed

### Possible responses

- `ACK`

---

## `REPAIR`

Tells a replica to repair a stale value.

**From**: coordinating replica

**To**: multiple replicas

### Arguments

- `k`: key of the stale value

- `v`: new value

- `ts`: timestamp of the new value

### Possible responses

No required response

---

## `SYNC`

Tells another replica some values stored in it.

**From**: replica

**To**: replica

### Arguments

- List of all elements `{k, ts}`:
  
  - `k`: key of the value
  
  - `ts`: timestamp of the stored value

### Possible responses

- `UPDATE`: like the `REPAIR` message, but with a list of values instead of just one

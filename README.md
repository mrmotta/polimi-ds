# Distributed Systems Project | A.Y. 2022-2023

Project developed by [Riccardo Motta](https://github.com/mrmotta/), [Pierluigi Negro](https://github.com/Gonduls/) and [Niccolò Nicolosi](https://github.com/NiccoloN/).

## The project: quorum-based replicated datastore

**Note**: the following specification is directly extracted from the one provided by the course teachers.

You are to implement a distributed key-value store that accepts two operations from clients:

- `put(k, v)` inserts/updates value `v` for key `k`;

- `get(k)` returns the value associated with key `k` (or `null` if the key is not present).

The store is internally replicated across $N$ nodes (processes) and offers sequential consistency using a quorum-based protocol. The read and write quorums are configuration parameters.

The project can be implemented as a real distributed application (for example, in Java) or it can be simulated using OmNet++. In the first case, you are allowed to use only basic communication facilities (that is, sockets and RMI in the case of Java).

### Assumptions

- Processes and links are reliable (use TCP to approximate reliable links and assume no network partitions can occur).

# `wsh` &mdash; A Very Small Shell

What happens when you execute a command like:

```shell
$ grep -i quux baz.txt | sort | wc -l
```

I knew the answer to this question _in theory_, but how much is that worth?
Hence this small project, a minimal `sh` clone.

The implementation is conventional, matching what you'll find in most textbooks
or the source code of `xv6`.
But even with lots of examples and reference material I still encountered a
number of interesting bugs.
For instance, it's difficult to debug with `printf` when file descriptors are
being swapped out.

Aside from gaining a better understanding of how a shell might compose child
processes, I learned a bit about _arena allocation_.
The idea is that instead of allocating and freeing dynamic storage using the
estimable `malloc` and `free`, we allocate blocks of space on our own, return
pointers into this space when needed, and free the allocated storage in one fell
swoop once none of the objects it references are needed any more.
The main advantage is that we don't need to chase pointers while `free`ing
allocations after they're no longer needed: we just free the entire block.
So long as all of the objects we're interested in storing have the same
lifetime (and we don't `realloc` too much) this is an efficient and foolproof
memory management technique.

In `wsh` I've taken an even lazier way out (no one is surprised): the arena is a
statically-allocated 4096-byte block.
If you use more than 4096 bytes, you're out of luck.
With this simplification, the arena implementation fits on a postcard:

```c
#define ARENA_SIZE 4096

static char arena_data[ARENA_SIZE];

static int arena_offset = 0;

void *arena_alloc(size_t size) {
        if (arena_offset + size >= ARENA_SIZE) {
                return NULL;
        }

        void *ptr = (void *) (arena_data + arena_offset);
        arena_offset += size;
        return ptr;
}

void *arena_realloc(void *ptr, size_t old_size, size_t new_size) {
        void *new_ptr = arena_alloc(new_size);
        if (!new_ptr) {
                return NULL;
        }
        memcpy(new_ptr, ptr, old_size);
        return new_ptr;
}

void reset_arena(void) {
        memset(arena_data, 0, ARENA_SIZE);
        arena_offset = 0;
}
```

So while parsing a command, we call `arena_alloc` whenever we want some space to
store an AST node or string.
We "free" this data by calling `reset_arena` after the command has been
executed, making it available for subsequent commands.
The `arena_realloc` function is used when collecting the argument vector
(`argv`) for "exec" commands like `/usr/bin/grep -i quux`: we don't know how
many arguments we're going to have in advance, so we guess some reasonable value
(in this case 4), and `realloc` the running list if we need more than that.

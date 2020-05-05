---
title: "Assignment vLast: Where’s the file?"
author:
  - Sam Olagun
  - Maanya Tandon
urlcolor: blue
geometry: margin=2cm
---

| Make Targets        | Description                                                                            |
| ------------------- | -------------------------------------------------------------------------------------- |
| `make all`          | Builds the server, client and tests and puts them in folders. It also builds this PDF. |
| `make test`         | Builds the server and put it in the `test` folder.                                     |
| `make clean`        | Removes server, client, and test folders. It also deletes this PDF.                    |
| `make build_client` | Builds the client, `wtf_client.c`, and moves it to the `client` folder.                |
| `make build_server` | Builds the server, `wtf_server.c`, and moves it to the `server` folder.                |
| `make build_test`   | Builds the tests, `wtf_test.c`, and moves them to the `test` folder.                   |

| Command                      | Description                                                     |
| ---------------------------- | --------------------------------------------------------------- |
| `./WTF add <project> <file>` | Adds a file to the client _.Manifest_ for a specific project.   |
| `./WTF checkout <project>`   | Downloads a project to the client from the server if it exists. |

| | |

- checkout
- commit
- create
- currentversion
- destroy
- history
- push
- remove
- rollback
- update
- upgrade

# Program Design

## Modularity

We knew that our project was going to be huge ahead of time so we took the time to organize it into modules and separate the interae from the code. we also made make targets to compile quickly and making wtftest easy to make.

## Protocol

C's standard socket library is very primitive. It makes sending bytes with a known length simple, but makes sending files and data fields more difficult. Because this assignment required us to send both files and data fields, we created a protocol for communicating this type of information.

Our "protocol" is the `Request` grammar and the `Response` grammar. These grammars specify how each `Request` and `Response` sent will appear in memory. Before I describe how we implemented `Request` and `Response`, here are the grammars for each. We arrived at these two grammars by modifying the recommended protocol in the assignment description.

### Request

```md
<message> ":"
<status_code> ":"
<command_name> ":"
<project_name> ":"
<project_version> ":"
<file_count> ":"
<file_path> ":"
<file_version> ":"
<file_hash> ":"
<file_size> ":"
<file_bytes>
```

### Response

```md
<message> ":"
<status_code> ":"
<project_version> ":"
<file_count> ":"
<file_path> ":"
<file_version> ":"
<file_hash> ":"
<file_size> ":"
<file_bytes>
```

To implement our protocol, we wrote _write_ and _read_ functions that wrote a `Request`/`Response` to a file descriptor and read a `Request`/`Response` from a socket file descriptor respectively. We also created `Request` and `Response` structures to match our protocol.

```c
// Taken from `includes/src/request.h`.
struct Request {
  char* message;
  int status_code;
  char* command_name;
  char* project_name;
  int project_version;
  int file_count;
  FileList* filelist;
};

Request* request_read(int fd);
void request_write(int fd, Request* request);

// Taken from `includes/src/response.h`.
struct Response {
  char* message;
  int status_code;
  int project_version;
  int file_count;
  FileList* filelist;
};

Response* response_read(int fd);
void response_write(int fd, Response* response);
```

Writing code this way allowed us to focus more on what we were sending and less on how we were sending it. It also made debugging easier because we created `request_log` and `response_log` functions to log reads and writes.

## Threading and Mutexes

outline:

a new thread is launched on every new conection that i acceped in terver_wait

mutexes are hanlded smiilarly acroess the board except fro create adn delete bcuawe they add and reove mouexs resplectivey.

we create amutex list with helpful erros for deteching deadlocks while testing

we als oreturn booleans to detect fi locks an unlocks are boh and successful and legal

```c
struct MutexList {
  char *project_name;
  bool is_locked;
  pthread_mutex_t *lock;
  struct MutexList *next;
};
```

## Testing and

```bash
make -s test_name
```

## Storing files in memory

The only files read are those listed in `.Manifest`. To send files, read in the manifest, then read file data into every file listed in the manifest.

```c
// Read in the manifest
Manifest* manifest = manifest_read("projects/test_project");

// Get files listed in the manifest
// Each file contains `file_name`, `file_version` and `file_hash`. See `filelist.h`
FileList* manifest_files = manifest->filelist;

// Read file data into every listed file
// Each file now now also contains `file_size` and `file_bytes`. See `filelist.h`
FileList* files_with_data = filelist_readbytes("projects/test_project", manifest_files);
```

## Sending files

After successfuly storing files inside of a `FileList` linked list, add them to a request to send them. Open [request.h](src/request.h) to see what else that a request can contain.

```c
// Read in manifest
Manifest* manifest = manifest_read("projects/test_project");

// Get files listed in the manifest
FileList* manifest_files = manifest->filelist;

// Read file data into every listed file
FileList* files_with_data = filelist_readbytes("projects/test_project", manifest_files);

// Create request
Request* request = request_new();

// Send files with request
request->filelist = files_with_data;

Response* response = client_send(request);
```

## Installing OpenSSL

Taken from [here](https://stackoverflow.com/questions/35129977/how-to-install-latest-version-of-openssl-mac-os-x-el-capitan).

```
brew update
brew install openssl
echo 'export PATH="/usr/local/opt/openssl/bin:$PATH"' >> ~/.bash_profile
source ~/.bash_profile
```

## Protocol

### Response Protocol

```md
<message>:
<status_code>:

<project_version>:
<file_count>:
<file_path>:
<file_version>:
<file_hash>:
<file_size>:
<file_removed>:
<file_bytes>
```

### Request Protocol

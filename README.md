# HTTP Server

The main point of this project is that it's a personal study into backend. As a result, the frontend isn't very pretty, but it is functional for what is intended to do.

## Current Functionality:
  - Accepts basic `GET` requests and will serve static page responses if the file exists. Otherwise, it sends back a 404 page.
    - It does not send back a header to the user agent, only the actual resource requested.
    - This works for Firefox and I'm sure most other web browsers, but on `curl`, you need to add `--http0.9` for it to accept the resource.
    - It's a priority to eventually add a header to these responses, but it's just me working on this, so it's gonna take a while.
  
  - Working on features to read and respond to `POST` requests in branch `post`.
  
  - Threads are not used currently, and no network timeouts have been programmed in yet. That's high on the priority list after finishing POST functionality.

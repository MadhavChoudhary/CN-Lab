# Assignment 2

***Note: Sample Images in `Images/` directory***
 
## As Client

### Steps
```
gcc client.c -o client
./client <hostname> <port> <path> <mode>
```
#### Mode 0
Simple retrieves the requested file from the host, prints the headers in the terminal and saves the content in a file with the same filename and extension.

#### Mode 1
Does Mode 1 and additionally tries to get other src files like other html,css,image files embedded in the base html file requested in separate connections, one by one and stores them locally

#### Mode 2
Does Mode 1 and then also displays the preview of the page in the terminal (like a command line browser!) along with images.  
Uses `imgcat` for images and `w3m` for html dumping.

### Working
1. Gets the ip from the hostname by using `getadddrinfo`
2. Creates a tcp socket connection to that ip and port and asks for the requested file.
3. The reply is received byte-by-byte and the header part is striped away by identifying `\r\n\r\n`. The later part is writen into a file with the same name and extension.
4. Incase of mode 1, if the file is a html, then the `src=` tags are found and the corresponding source url is parsed and again requested through a new connection and the same process repeats for that object.
5. In case of mode 2, thrid party tools are used to display the page interactively.

***Most of the pages on net are https, so probably won't work with this code, since this is only for http***

## As Server

### Steps
```
gcc server.c -o server
./server <port>
```
The root directory of the server is `./server/`  
Server is capable of receiving GET and POST requests.

### Working
1. When a new request is recieved, the requested file name with extension is parsed.
2. Based on the file size, file extension, the header of the reply from server is formed.
3. The header is not hard coded, it is generalised. The mime_type is assumed by matching the extension with existing mime types in `mime_types.txt`
4. The file is read, packet is formed and is sent to the client.

### Example 
```
<!DOCTYPE html>
<html>
<body>

<h2>This is my test website</h2>
<p>This is a html document and this is my image below.</p>

<img src="scene.jpg" alt="scene" width="650" height="500">

</body>
</html>
```
If this is the index.html on the server side, the output of mode 2 client is shown in images. You can always comment the header printing to make them less obstrusive in between the html file.
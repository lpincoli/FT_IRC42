<body>
    <h1>Ft_IRC</h1>
    <p>ft_irc is a project where you create your own Internet Relay Chat (IRC) server. This is a remarkable opportunity to learn about network protocols, non-blocking I/O operations, and low-level programming in C++.</p>
    <h2>Table of Contents</h2>
    <ol>
        <li><a href="#about">About</a></li>
        <li><a href="#installation">Installation</a></li>
        <li><a href="#usage">Usage</a></li>
    </ol>
    <h2 id="about">About</h2>
    <p>ft_irc is a project where you'll build an IRC server in C++. The server should support multiple clients, allow for authentication, setting a nickname, a username, joining a channel, and sending/receiving private messages.</p>
    <h2 id="installation">Installation</h2>
    <ol>
        <li>Clone the repository: <pre><code>git clone https://github.com/lpincoli/FT_IRC42.git</code></pre></li>
        <li>Move to the project's directory: <pre><code>cd FT_IRC42</code></pre></li>
        <li>Compile the project: <pre><code>make</code></pre></li>
    </ol>
    <h2 id="usage">Usage</h2>
    <p>Launch your IRC server from the command line after running 'make'. You'll need to provide a port number and a password for client connections. Example:</p>
    <pre><code>./ircserv 6667 mypassword</code></pre>
    <p>⚠️ Note that this is only a server, you'll need a client to connect to it if you want to try the program properly.</p>
    <h3>Features:</h3>
    <ul>
        <li>Support multiple clients</li>
        <li>Authentication, set a nickname, a username</li>
        <li>Join a channel</li>
        <li>Send and receive private messages</li>
        <li>Distinguish between operators and regular users</li>
        <li>Implement specific commands for channel operators</li>
    </ul>
    
<h2>Subject</h2>

<p>For more information about the project, read the subject file.</p>
</body>
</html>

require 'rubygems'
require 'eventmachine'
require 'evma_httpserver'
require 'websocket-eventmachine-server'

module WebSocket
  module EventMachine
    class Server

      # Where can we find the directory of valid SSL client certificates?
      CLIENT_CERT_DIR = 'config/tls/valid-client-certs'

      # Called for every client certificate submitted.  Do nothing and
      # wait for ssl_handshake_completed to verify the final peer certificate
      def ssl_verify_peer(cert)
        return true
      end

      # Check if the client submitted a valid certificate that is nonetheless
      # not in our directory of valid certificates.  If so, close the connection.
      # If there is no valid client certificate, the client may be a browser that
      # can authenticate via a cookie (see websocket onopen method below)
      def ssl_handshake_completed
        return unless @tls_options[:verify_peer]
        @client_cert = get_peer_cert
        close_connection if @client_cert && !valid_client?(@client_cert)
      end

      # Check if given certificate, in PEM format, is one of the ones in our
      # valid certificate directory.
      def valid_client? pem
        cert = OpenSSL::X509::Certificate.new pem

        @valid_certs ||= Dir["#{CLIENT_CERT_DIR}/*.crt"].collect do |pemfile|
          { name: File.basename(pemfile),
            cert: OpenSSL::X509::Certificate.new(File.read(pemfile)) }
        end

        @valid_certs.each do |cert_hash|
          if cert_hash[:cert].issuer == cert.issuer &&
             cert_hash[:cert].serial == cert.serial
            return true
          end
        end

        # The supplied certificate doesn't match any in client_cert_dir
        return false
      end

      # Check if Cookie header submitted with request to upgrade the connection
      # to websockets is a valid Rails session cookie
      def valid_cookie?
        cookie = @handshake.headers['Cookie']
        puts "Cookie: #{cookie}"
        return false unless cookie
      end

      def client_cert
        @client_cert
      end

    end
  end
end

class VehicleConnector

  def initialize
    # SSL key and certificate locations
    @server_key = 'config/tls/server.key'
    @server_crt = 'config/tls/server.crt'
    @ws_port = 8126
    # Array of current connections
    @websocket_connections = []
  end

  class HttpServer < EventMachine::Connection
    include EventMachine::HttpServer

    def process_http_request
      puts @http_post_content
      puts "Something received on 3001"
      response = EventMachine::DelegatedHttpResponse.new(self)
      response.status = 200
      response.content_type 'text/html'
      response.content = '<center><h1>Hi there</h1></center>'
      response.send_response
      @websocket_connections.each do |socket|
        socket.send(@http_post_content)
        
      end
    end
  end


  def start
    EventMachine.run do
    
    @msg

      @websocket_connections.each do |ws|
        ws.close
      end

      WebSocket::EventMachine::Server.start(host: "0.0.0.0", port: @ws_port,
           secure: true, tls_options: { private_key_file: @server_key,
                   cert_chain_file: @server_crt, verify_peer: true}) do |ws|

        ws.onopen do
          puts "Websocket connection opened at #{@ws_port}"
          puts "Certificate: #{ws.client_cert}"
          if ws.client_cert.nil? && !ws.valid_cookie?
            ws.close_connection
          end
          @websocket_connections << ws
          @onclose = 0  
          #Initialized the arrays here, but not sure if this is the right place. 
	  @time_stamp_array = Array.new 
          @lat_dis_array = Array.new
          @lane_change_array = Array.new	
 
        end

        ws.onmessage do |msg|
          puts msg
          @msg = msg
          # Check validity

          # Broadcast to all browser clients
          @websocket_connections.each do |socket|
            socket.send(msg)
          end
          # Add call to accumulate this data item and invoke the appropriate Driver
          # instance to update statistics and save to the database
          queue(msg)
          update

        end

        ws.onclose do
          @onclose = 1
          update
          puts "Websocket connection closed at #{@ws_port}"
          @websocket_connections.delete(ws)
        end

      end

      EventMachine.start_server('0.0.0.0', 3001, HttpServer) do |conn|
        conn.instance_variable_set(:@websocket_connections, @websocket_connections)
      end

    end

  end




  def queue(msg)
    
    # Parse message
    spl = msg.split(",")
    if spl[0]=="f"
      if spl[1].to_i > 0 
        @time_stamp_array.push(spl[2].to_i)
        @lat_dis_array.push(spl[3])
        @lane_change_array.push(spl[4])
      end 
    end
   
  end


  def update
   
    # Ask Driver instance to update its statistics

    if @time_stamp_array.last.to_i - @time_stamp_array.first.to_i > 2000 || @onclose.to_i == 1

      puts "#########################################"
      # Find Driver instance
      @driver = Driver.first
      #pass the arrays into the driver instance
      @driver.update(@time_stamp_array, @lat_dis_array, @lane_change_array)

      puts "######################################### "
      @time_stamp_array.clear
      @lat_dis_array.clear
      @lane_change_array.clear 
      @onclose = 0 
    end
		
  end

end

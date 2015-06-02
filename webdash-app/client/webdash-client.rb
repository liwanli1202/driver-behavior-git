require 'rubygems'
require 'bundler/setup'
require 'faye/websocket'
require 'eventmachine'

if ARGV.length == 0 || ARGV.length > 3
  puts "Usage: webdash-client.rb <URL> <client key file> <client certificate file>"
  puts "Example: webdash-client.rb wss://0.0.0.0:8126 webdash-client-1.key webdash-client-1.crt"
  exit
end

class StdinHandler < EM::Connection
  include EM::Protocols::LineProtocol

  def initialize(ws)
    @ws = ws
  end

  def receive_line line
    @ws.send line
  end

  def receive_data data
    if data == "\x00"
      @ws.close
      EM.stop
    else
      super data
    end
  end

  def add_to_time_stamp_array time_stamp
    @timeStamp.push time_stamp	
  end	
end


EM.run {
  url = ARGV[0]
  ws = if ARGV.length > 1
    Faye::WebSocket::Client.new(url, nil,
                                tls: { private_key_file: ARGV[1],
                                cert_chain_file: ARGV[2] })
  else 
    Faye::WebSocket::Client.new(url, nil)
  end

  ws.onopen = lambda do |event|
    p [:open, ws.headers]

  end

  ws.onclose = lambda do |close|
    p [:close, close.code, close.reason]
    EM.stop
  end

  ws.onerror = lambda do |error|
    p [:error, error.message]
  end

  ws.onmessage = lambda do |message|
    # Right now the server is echoing
    #p [:message, message.data]
  end

  EM.open_keyboard(StdinHandler, ws)
}

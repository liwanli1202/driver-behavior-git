
require 'vehicle_connector/vehicle_connector.rb'

namespace :vehicle_connector do
  task :start => :environment do
    begin
      pid = `lsof -F -i :8126 | grep '^p' | sed 's/^p//'`
      Process.kill 2, pid.to_i unless pid.nil? or pid.empty?
      vehicle_connector = VehicleConnector.new
      vehicle_connector.start
    rescue Interrupt, SignalException
      exit 0
    end
  end

  task :stop do
    pid = `lsof -F -i :8126 | grep '^p' | sed 's/^p//'`
    Process.kill 2, pid.to_i unless pid.nil? or pid.empty?
  end
end

multitask run_dev_servers: %w[rails_s vehicle_connector:start]

task :rails_s do
  pid = `lsof -F -i :3000 | grep '^p' | sed 's/^p//'`
  Process.kill 2, pid.to_i unless pid.nil? or pid.empty?
  sh "rails s"
end

task :fake_vehicle_connector do
  exe = '../../clientSocket/webclient/webdash-client.rb'
  ws_uri = 'wss://0.0.0.0:8126'
  client_key = 'config/tls/test-client/webdash-client-1.key'
  client_crt = 'config/tls/test-client/webdash-client-1.crt'
  fake_client_log = '../../../test/testFiles/DriverAwarenessHeadlessTest-log.txt'
  system("bundle exec ruby #{exe} #{ws_uri} #{client_key} #{client_crt} < #{fake_client_log}")
end

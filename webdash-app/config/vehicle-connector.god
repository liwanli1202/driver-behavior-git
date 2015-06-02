working_dir = "/home/deploy/webdash/current"

module God
  module Conditions
    class RestartFileTouched < PollCondition
      attr_accessor :restart_file
      def initialize
        super
      end

      def process_start_time
        Time.parse(`ps -o lstart -p #{self.watch.pid} --no-heading`)
      end

      def restart_file_modification_time
        File.mtime(self.restart_file)
      end

      def valid?
        valid = true
        valid &= complain("Attribute 'restart_file' must be specified", self) if self.restart_file.nil?
        valid
      end

      def test
        process_start_time < restart_file_modification_time
      end

    end
  end
end

God.watch do |w|
  w.name = "vehicle_connector"
  w.env = {"RAILS_ENV" => "production"}
  w.dir = working_dir
  w.log = File.join(working_dir, "log/vehicle_connector.log")
  w.start = "bundle exec rake vehicle_connector:start"

  w.keepalive

  w.restart_if do |restart|
    restart.condition(:memory_usage) do |c|
      c.interval = 5.seconds
      c.above = 500.megabytes
      c.times = [3, 5] # 3 out of 5 intervals
    end
    restart.condition(:restart_file_touched) do |c|
      c.interval = 5.seconds
      c.restart_file = File.join(working_dir, 'tmp', 'restart.txt')
     end
  end
end

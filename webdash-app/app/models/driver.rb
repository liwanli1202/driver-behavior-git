class Driver < ActiveRecord::Base

  def update(time_stamp_array, lat_dis_array, lane_change_array)

    @@lat_dis_array = Array.new	

    #updates the most recent lane change speed
    #DATABASE COLUMN : most_recent_lane_change_speed
    def update_lane_change_speed(previous_lane_change_speed, lane_change_array)
      @most_recent_lane_change_speed = previous_lane_change_speed
      lane_change_array.each do |element|
        if element.to_i != 0
          @most_recent_lane_change_speed = element.to_i;
        end
      end
      return @most_recent_lane_change_speed.to_i
    end

    #updates the total time the driver has driven. This is used to calculate the lane change frequency.  
    #DATABASE COLUMN : time_driven
    def update_total_time(last_updated_total_time_driven, time_stamp_array)
      return last_updated_total_time_driven.to_i + (time_stamp_array.last.to_i - time_stamp_array.first.to_i)
    end

    #returns the number of total lane changes of the driver. This is used to calculate the lane change frequency.
    #DATABASE COLUMN : number_of_lane_changes
    def update_number_of_lane_changes(number_of_previous_lane_changes, lane_change_array)
      @number_of_lane_changes = 0
      lane_change_array.each do |element|
        if element.to_i != 0
          @number_of_lane_changes = @number_of_lane_changes + 1;
        end
      end

      return @number_of_lane_changes.to_i + number_of_previous_lane_changes.to_i
    end

    #updates the lane change frequency | UNITS : lane changes per min
    #DATABASE COLUMN : lane_change_frequency
    def update_lane_change_frequency(number_of_lane_changes, time_driven)
      return ((number_of_lane_changes.to_f * 1000 * 60) / time_driven.to_f).to_f
    end

    #updates the sum of all lane change speeds. This is used to calculate the average lane change speed later.
    #DATABASE COLUMN : lane_change_speed_sum
    def update_sum_of_lane_change_speeds(previous_sum_of_lane_change_speeds, lane_change_array)
      @most_recent_lane_change_speed = 0
      lane_change_array.each do |element|
        if element.to_i != 0
          @most_recent_lane_change_speed = @most_recent_lane_change_speed + element.to_i;
        end
      end
      return @most_recent_lane_change_speed.to_i + previous_sum_of_lane_change_speeds.to_i
    end

    #updates the average lane change Speed
    #DATABASE COLUMN : avg_lane_change_speed
    def update_average_lane_change_speed(lane_change_speed_sum, number_of_lane_changes)
      return (lane_change_speed_sum.to_f / number_of_lane_changes.to_f).to_f
    end

    #updates stability
    #DATABASE COLUMN : stability
    def update_stability(lat_dis_array)
      @mean = lat_dis_array.sum.to_f / lat_dis_array.size.to_f
      @temp = 0
      lat_dis_array.each do |element|
        @temp = @temp.to_f + ((@mean.to_f - element.to_f) * (@mean.to_f - element.to_f))
      end


      return @temp.to_f / lat_dis_array.size.to_f
    end

    #converts a string into array. 
    def string_to_array(lat_dis_string)
      spl = lat_dis_string.split("|")
      @@lat_dis_array.push(spl)
      return @@lat_dis_array   		
    end		
	


    #updates average stability
    #DATABASE COLUMN : avg_stability
    def update_avg_stability(sum_of_stability, number_of_frames, lat_dis_array)
      @sum = 0
      @size = 0
      @total_stability = 0
      lat_dis_array.each do |element|
        @sum = @sum.to_f + element.to_f
        @size = @size.to_f + 1.0
        @mean = @sum.to_f / @size.to_f
        @stability = ((@mean.to_f - element.to_f) * (@mean.to_f - element.to_f)) / @size.to_f
        @total_stability = @total_stability.to_f + @stability.to_f

      end
      self.number_of_frames = self.number_of_frames.to_i + @size.to_i
      self.sum_of_stability = self.sum_of_stability.to_f + @total_stability.to_f
      self.save
      return @total_stability.to_f / number_of_frames.to_f
    end

    def update_lat_dis_string(lat_dis_array, time_stamp_array )
      s = ""
      if time_stamp_array.first != 0 #filters out the first message starting from time = 0  
      	lat_dis_array.each do |element|
          s = s + element.to_s + "|"
        end	
      end
      return s.to_s			
    end 


    #update the database   
    self.most_recent_lane_change_speed = update_lane_change_speed(most_recent_lane_change_speed, lane_change_array)
    self.time_driven = update_total_time(time_driven, time_stamp_array)
    self.number_of_lane_changes = update_number_of_lane_changes(number_of_lane_changes, lane_change_array)
    self.lane_change_frequency = update_lane_change_frequency(number_of_lane_changes, time_driven)
    self.lane_change_speed_sum = update_sum_of_lane_change_speeds(lane_change_speed_sum, lane_change_array)
    self.avg_lane_change_speed = update_average_lane_change_speed(lane_change_speed_sum, number_of_lane_changes)
    self.stability = update_stability(lat_dis_array)
    self.avg_stability = update_avg_stability(sum_of_stability, number_of_frames, lat_dis_array)
    self.lat_dis_string = update_lat_dis_string(lat_dis_array, time_stamp_array)	
    puts string_to_array(self.lat_dis_string)
    self.save

    #prints to console
    puts "Most recent lane change speed is: #{most_recent_lane_change_speed}"
    puts "Time Driven                     : #{time_driven}"
    puts "Number of lane changes          : #{number_of_lane_changes}"
    puts "Lane change frequency           : #{lane_change_frequency} per min"
    puts "Sum of lane Change speeds       : #{lane_change_speed_sum}"
    puts "Average lane change speed       : #{avg_lane_change_speed}"
    puts "Stability                       : #{stability}"
    puts "sum_of_stabiliy                 : #{sum_of_stability}"
    puts "number of frames                : #{number_of_frames}"
    puts "Average Stability               : #{avg_stability}"



  end

end

class AddNumberOfFramesToDriver < ActiveRecord::Migration
  def change
    add_column :drivers, :number_of_frames, :integer
  end
end

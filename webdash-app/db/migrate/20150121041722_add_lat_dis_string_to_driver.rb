class AddLatDisStringToDriver < ActiveRecord::Migration
  def change
    add_column :drivers, :lat_dis_string, :text
  end
end

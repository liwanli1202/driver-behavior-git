class AddTimeDrivenToDriver < ActiveRecord::Migration
  def change
    add_column :drivers, :time_driven, :integer
  end
end

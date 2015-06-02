class AddStabilityToDriver < ActiveRecord::Migration
  def change
    add_column :drivers, :stability, :float
  end
end

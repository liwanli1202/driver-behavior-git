class AddAvgStabilityToDriver < ActiveRecord::Migration
  def change
    add_column :drivers, :avg_stability, :float
  end
end

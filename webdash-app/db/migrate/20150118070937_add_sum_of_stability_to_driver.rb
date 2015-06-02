class AddSumOfStabilityToDriver < ActiveRecord::Migration
  def change
    add_column :drivers, :sum_of_stability, :float
  end
end

class AddWorkoutIdToFunction < ActiveRecord::Migration
  def self.up
    add_column :functions, :workout_id, :integer
  end

  def self.down
    remove_column :functions, :integer
  end
end

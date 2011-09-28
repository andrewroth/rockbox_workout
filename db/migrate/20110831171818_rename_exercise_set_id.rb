class RenameExerciseSetId < ActiveRecord::Migration
  def self.up
    rename_column :functions, :exercise_set_id, :set_id
  end

  def self.down
    rename_column :functions, :set_id, :exercise_set_id
  end
end

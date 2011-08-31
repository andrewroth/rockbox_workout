class RenameWorkoutSetIdToExerciseSetId < ActiveRecord::Migration
  def self.up
    rename_column :set_log_entries, :workout_set_id, :set_log_entry_id
  end

  def self.down
    rename_column :set_log_entries, :set_log_entry_id, :workout_set_id
  end
end

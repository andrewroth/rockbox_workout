class AddExtraTimestampsToLogs < ActiveRecord::Migration
  def self.up
    add_column :workout_dates, :started_at, :datetime
    add_column :workout_dates, :finished_at, :datetime
    #add_column :workout_log_entries, :started_at, :datetime
    #add_column :workout_log_entries, :finished_at, :datetime
    add_column :exercise_log_entries, :started_at, :datetime
    add_column :exercise_log_entries, :finished_at, :datetime
    add_column :exercise_log_entries, :last_completed_at, :datetime
    add_column :set_log_entries, :started_at, :datetime
    add_column :set_log_entries, :completed_at, :datetime
    add_column :set_log_entries, :done_rested_at, :datetime
  end

  def self.down
    remove_column :workout_dates, :started_at
    remove_column :workout_dates, :finished_at
    #remove_column :workout_log_entries, :started_at
    #remove_column :workout_log_entries, :finished_at
    remove_column :exercise_log_entry, :started_at
    remove_column :exercise_log_entry, :finished_at
    remove_column :exercise_log_entry, :last_completed_at
    remove_column :set_log_entry, :started_at
    remove_column :set_log_entry, :completed_at
    remove_column :set_log_entry, :done_rested_at
  end
end

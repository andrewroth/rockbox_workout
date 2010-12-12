class SetLogEntry < ActiveRecord::Base
  def self.read_csv
    sle = nil
    f = File.read(Rails.root.join("transfer/set_logs.csv"))
    f.split("\n").each do |line|
      name, type, value = line.split(",")
      case name
      when 'created_at_int'
        sle.save! if sle
        sle = SetLogEntry.find_or_create_by_created_at(Time.at(value.to_i))
      when 'started_at_int'
        sle.started_at = Time.at(value.to_i) if value != '0'
      when 'completed_at_int'
        sle.completed_at = Time.at(value.to_i) if value != '0'
      when 'done_rested_at_int'
        sle.done_rested_at = Time.at(value.to_i) if value != '0'
      when 'exercise_set_id'
        sle.workout_set_id = value
      when 'exercise_log_entry_created_at_int'
        #throw ExerciseLogEntry.find_by_created_at(Time.at(value.to_i)).inspect
        sle.exercise_log_entry_id = ExerciseLogEntry.find_by_created_at(Time.at(value.to_i)).id
        puts sle.exercise_log_entry_id.inspect
      #when 'n'
      #  sle.n = value
      end
      sle.save! if sle
    end
  end

end

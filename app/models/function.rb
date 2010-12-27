class Function < ActiveRecord::Base
  include Csv
  belongs_to :exercise_set

  def self.write_csv
    super do
      WorkoutDate.where("finished_at IS NULL").collect(&:workout).flatten.collect(&:exercises).flatten.collect(&:exercise_sets).flatten.collect(&:functions).uniq
    end
  end
end

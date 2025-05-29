export type ClockSettings = {
  animation: {
    // The extra property is neccessary as ConfigDB expects it for tagging the oneof union
    Static: { name: "static"; color: string };
  };
};

export type GeneralSettings = {
  maxBrightness: number;
  nightMode?: {
    startTime?: string;
    endTime?: string;
    dimmingAmount: number;
  };
  timezone: string;
};

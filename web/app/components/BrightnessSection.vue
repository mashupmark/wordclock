<script lang="ts" setup>
import * as v from "valibot";
import type { GeneralSettings } from "~~/shared/types/api";

const toast = useToast();

const schema = v.pipe(
  v.object({
    maxBrightness: v.pipe(
      v.number(),
      v.integer(),
      v.minValue(1),
      v.maxValue(255)
    ),
    enableNightMode: v.boolean(),
    startTime: v.optional(v.string()),
    endTime: v.optional(v.string()),
    dimmingAmount: v.optional(v.number()),
  }),
  v.forward(
    v.check(
      (input) =>
        !input.enableNightMode ||
        (!!input.startTime && !!input.endTime && !!input.dimmingAmount)
    ),
    ["enableNightMode"]
  )
);

type Schema = v.InferOutput<typeof schema>;

const state = reactive<Schema>({
  maxBrightness: 255,
  enableNightMode: false,
  startTime: "23:00",
  endTime: "06:00",
  dimmingAmount: 0.5,
});
const isValid = computed(() => v.safeParse(schema, state).success);

const { status, data } = useFetch<GeneralSettings>("/api/settings", {
  server: false,
});
watchEffect(() => {
  if (!data.value) return;
  state.maxBrightness = data.value.maxBrightness;

  state.enableNightMode = (data.value.nightMode?.dimmingAmount ?? 0) > 0;
  state.dimmingAmount = (data.value.nightMode?.dimmingAmount ?? 255) / 255;
  state.startTime = data.value.nightMode?.startTime;
  state.endTime = data.value.nightMode?.endTime;
});

const { status: saveStatus, execute: save } = useAsyncData(
  async () => {
    try {
      await $fetch("/api/settings", {
        method: "POST",
        body: {
          maxBrightness: state.maxBrightness,
          nightMode: {
            dimmingAmount: state.enableNightMode
              ? Math.floor((state.dimmingAmount ?? 0) * 255)
              : 0,
            startTime: state.startTime,
            endTime: state.endTime,
          },
        } satisfies Partial<GeneralSettings>,
      });
    } catch {
      toast.add({ title: "Failed to update brightness", color: "error" });
    }
  },
  { immediate: false }
);
</script>

<template>
  <SettingsSection
    title="Brightness"
    description="Set the maximum brightness of the leds"
    :schema
    :state
    :isSaving="saveStatus === 'pending'"
    :isDisabled="!isValid"
    @submit="save()"
  >
    <UFormField label="Max Brightness" required>
      <USlider
        v-model="state.maxBrightness"
        :min="1"
        :max="255"
        :disabled="status === 'pending'"
      />
    </UFormField>
    <div class="pt-4">
      <UFormField required>
        <USwitch
          label="Night mode"
          v-model="state.enableNightMode"
          :disabled="status === 'pending'"
        />
      </UFormField>
      <div v-if="state.enableNightMode" class="pt-4 flex gap-4">
        <UFormField label="Start time" required>
          <UInput type="time" v-model="state.startTime" />
        </UFormField>
        <UFormField label="End time" required>
          <UInput type="time" v-model="state.endTime" />
        </UFormField>
        <UFormField label="Dimm by" required>
          <UInputNumber
            v-model="state.dimmingAmount"
            :min="0.01"
            :max="1"
            :step="0.01"
            :formatOptions="{ style: 'percent' }"
          />
        </UFormField>
      </div>
    </div>
  </SettingsSection>
</template>

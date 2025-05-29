<script lang="ts" setup generic="TSchema extends ObjectSchema<any, any>">
import { type ObjectSchema, type InferOutput } from "valibot";
import type { FormSubmitEvent } from "#ui/types";

const props = defineProps<{
  title: string;
  description?: string;
  state: InferOutput<TSchema>;
  schema: TSchema;
  isSaving?: boolean;
  isDisabled?: boolean;
}>();

const emit = defineEmits<{
  submit: [data: FormSubmitEvent<InferOutput<TSchema>>];
}>();
</script>

<template>
  <UForm
    class="flex flex-col gap-2 p-4 border-b border-gray-200 dark:border-gray-800"
    :schema="props.schema"
    :state="props.state"
    @submit="emit('submit', $event)"
  >
    <UFormField
      :label="title"
      :description
      size="xl"
      class="pb-2 flex justify-between"
      :ui="{ label: 'font-semibold' }"
    >
      <UButton
        label="Save changes"
        type="submit"
        color="neutral"
        :loading="props.isSaving"
        :disabled="props.isDisabled"
      />
    </UFormField>

    <slot />
  </UForm>
</template>

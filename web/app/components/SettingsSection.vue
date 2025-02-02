<script lang="ts" setup generic="TSchema extends ObjectSchema<any, any>">
import { type ObjectSchema, type InferOutput, safeParser } from "valibot";
import type { FormSubmitEvent } from "#ui/types";

const props = defineProps<{
  title: string;
  description?: string;
  state: InferOutput<TSchema>;
  schema: TSchema;
  isSaving?: boolean;
}>();

const emit = defineEmits<{
  submit: [data: FormSubmitEvent<InferOutput<TSchema>>];
}>();
</script>

<template>
  <UForm
    class="flex flex-col gap-2 p-4 border-b border-gray-200 dark:border-gray-800"
    :schema="safeParser(props.schema)"
    :state="props.state"
    @submit="emit('submit', $event)"
  >
    <UFormGroup
      :label="title"
      :description
      size="xl"
      class="pb-2 flex justify-between"
      :ui="{ label: { base: 'font-semibold' } }"
    >
      <UButton
        label="Save changes"
        type="submit"
        color="black"
        :loading="props.isSaving"
      />
    </UFormGroup>

    <slot />
  </UForm>
</template>
